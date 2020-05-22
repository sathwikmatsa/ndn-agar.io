#include "game_server.hpp"
#include "./../shared/game_settings.hpp"
#include <csignal>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <yojimbo/yojimbo.h>

namespace {
static volatile std::sig_atomic_t running = 1;
}

void signal_handler(int) { running = 0; }

// null private key
static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = {0};

GameServer::GameServer(const yojimbo::Address &address)
    : adapter(this), server(yojimbo::GetDefaultAllocator(), DEFAULT_PRIVATE_KEY,
                            address, conn_config, adapter, 0.0f) {
  server.Start(MAX_PLAYERS);
  if (!server.IsRunning()) {
    throw std::runtime_error("Could not start server at port " +
                             std::to_string(address.GetPort()));
  } else {
    running = 1;
  }
  // print the port we got in case we used port 0
  char buffer[256];
  server.GetAddress().ToString(buffer, sizeof(buffer));
  std::cout << "Server is running at address: " << buffer << std::endl;
  flog = spdlog::get("flog");
  time = 0.0f;
  snapshot_id = 0;
}

void GameServer::client_connected(int client_index) {
  spdlog::info("new client ({}) is connected", client_index);
}

void GameServer::client_disconnected(int client_index) {
  spdlog::warn("client {} disconnected", client_index);
  int stats_size = state.players_stats.size();
  if (stats_size > client_index) {
    spdlog::warn("erased player {} stats", client_index);
    state.players_stats[client_index] = PlayerStats();
  }
  bool multicast = false;
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (server.IsClientConnected(i) && i != client_index) {
      DeadPlayerMessage *message = (DeadPlayerMessage *)server.CreateMessage(
          i, (int)GameMessageType::DEAD_PLAYER);
      message->player_index = client_index;
      server.SendMessage(i, (int)GameChannel::RELIABLE, message);
      multicast = true;
    }
  }
  if (multicast)
    spdlog::debug("multicasted dead player message");
}

void GameServer::run() {
  flog->info("server running");
  std::signal(SIGINT, signal_handler);
  float fixed_dt = 1.0f / 60.0f;
  while (running != 0) {
    double current_time = yojimbo_time();
    if (time <= current_time) {
      update(fixed_dt);
      time += fixed_dt;
    } else {
      yojimbo_sleep(time - current_time);
    }
  }
  stop();
}

void GameServer::update(float deltat) {
  // stop if server is not running
  if (!server.IsRunning()) {
    running = 0;
    return;
  }

  // update server and process messages
  server.AdvanceTime(server.GetTime() + deltat);
  server.ReceivePackets();
  process_messages();

  // ... process client inputs ...
  // ... update game ...
  // ... send game state to clients ...
  server.SendPackets();
}

void GameServer::process_messages() {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (server.IsClientConnected(i)) {
      for (int j = 0; j < conn_config.numChannels; j++) {
        yojimbo::Message *message = server.ReceiveMessage(i, j);
        while (message != NULL) {
          process_message(i, message);
          server.ReleaseMessage(i, message);
          message = server.ReceiveMessage(i, j);
        }
      }
    }
  }
}

void GameServer::process_message(int client_index, yojimbo::Message *message) {
  switch (message->GetType()) {
  case (int)GameMessageType::NEW_PLAYER:
    process_newplayer_message(client_index, (NewPlayerMessage *)message);
    break;
  case (int)GameMessageType::ATE_PELLET:
    process_atepellet_message(client_index, (AtePelletMessage *)message);
    break;
  case (int)GameMessageType::PLAYER_UPDATE:
    process_playerupdate_message(client_index, (PlayerUpdateMessage *)message);
    break;
  default:
    spdlog::error("unexpected message");
    break;
  }
}

void GameServer::process_newplayer_message(int client_index,
                                           NewPlayerMessage *message) {
  spdlog::info("newplayer: {}", message->player_name);
  // store new player info
  std::string name(message->player_name);
  uint8_t r = message->r;
  uint8_t g = message->g;
  uint8_t b = message->b;
  int n_players = state.players.size();
  if (n_players > client_index) {
    spdlog::debug("Inserting player into an already existing slot");
    state.players[client_index] = std::make_tuple(name, r, g, b, 0);
  } else {
    spdlog::debug("Creating new slot for the player");
    state.players.emplace_back(name, r, g, b, 0);
  }

  // send game info to new player
  if (server.IsClientConnected(client_index)) {
    GameInfoMessage *reply = (GameInfoMessage *)server.CreateMessage(
        client_index, (int)GameMessageType::GAME_INFO);
    reply->player_index = client_index;
    reply->players = std::move(state.get_players());
    reply->pellets = state.pellets;
    reply->viruses = state.viruses;
    server.SendMessage(client_index, (int)GameChannel::RELIABLE, reply);
    spdlog::info("sending game info");
  }

  // multicast new player info to other players
  bool multicast = false;
  for (int index = 0; index < MAX_PLAYERS; index++) {
    if (server.IsClientConnected(index) && index != client_index) {
      multicast = true;
      NewPlayerMessage *s_message = (NewPlayerMessage *)server.CreateMessage(
          index, (int)GameMessageType::NEW_PLAYER);
      s_message->player_index = client_index;
      strcpy(s_message->player_name, message->player_name);
      s_message->r = message->r;
      s_message->g = message->g;
      s_message->b = message->b;
      server.SendMessage(index, (int)GameChannel::RELIABLE, s_message);
    }
  }
  if (multicast)
    spdlog::info("multicasted new player {} message", client_index);
}

void GameServer::process_atepellet_message(int, AtePelletMessage *message) {
  auto [new_x, new_y] = state.relocate_pellet(message->pellet_id);
  // broadcast pellet relocation to all players
  for (int index = 0; index < MAX_PLAYERS; index++) {
    if (server.IsClientConnected(index)) {
      PelletRelocMessage *reply = (PelletRelocMessage *)server.CreateMessage(
          index, (int)GameMessageType::PELLET_RELOC);
      reply->pellet_id = message->pellet_id;
      reply->pos_x = new_x;
      reply->pos_y = new_y;
      server.SendMessage(index, (int)GameChannel::RELIABLE, reply);
    }
  }
}

void GameServer::process_playerupdate_message(int client_index,
                                              PlayerUpdateMessage *message) {
  int n_registered_players = state.players.size();
  if (client_index >= n_registered_players)
    return;
  if (message->seq_id > std::get<4>(state.players[client_index])) {
    spdlog::debug("player update [{}] : cells {}, ejectiles {}", client_index,
                  message->info.cells.size(), message->info.ejectiles.size());
    std::get<4>(state.players[client_index]) = message->seq_id;
    int n_stats = state.players_stats.size();
    if (n_stats <= client_index) {
      state.players_stats.resize(state.players.size());
    }
    state.players_stats[client_index] = std::move(message->info);

    // send snapshot
    SnapshotMessage *reply = (SnapshotMessage *)server.CreateMessage(
        client_index, (int)GameMessageType::SNAPSHOT);
    reply->id = ++snapshot_id;
    reply->stats = state.players_stats;
    server.SendMessage(client_index, (int)GameChannel::UNRELIABLE, reply);
    spdlog::debug("sent snapshot to {}", client_index);

  } else {
    spdlog::debug("received older player update message of client {}",
                  client_index);
  }
}

void GameServer::stop() {
  spdlog::critical("server is stopped.");
  server.Stop();
}
