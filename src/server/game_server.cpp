#include "game_server.hpp"
#include "./../shared/game_settings.hpp"
#include "./../shared/game_messages.hpp"
#include "./../shared/serialization.hpp"
#include <csignal>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

using namespace ndn;

namespace {
static volatile std::sig_atomic_t running = 1;
}

void signal_handler(int) { running = 0; }

GameServer::GameServer(Name server_prefix, int n_players) {
  flog = spdlog::get("flog");
  time = 0.0f;
  snapshot_id = 0;
  lobby_capacity = n_players;
  connected_players = 0;
  game_started = false;
  finished = 0;
  interest_checklist.clear();

  // setup interest filters
  // join room
  join_handler = face.setInterestFilter(
    Name(server_prefix).append("join"),
    std::bind(&GameServer::join_interest, this, _1, _2),
    nullptr,
    std::bind(&GameServer::on_register_failed, this, _1, _2)
  );
  // messages
  message_handler = face.setInterestFilter(
    Name(server_prefix).append("message"),
    std::bind(&GameServer::process_message, this, _1, _2),
    nullptr,
    std::bind(&GameServer::on_register_failed, this, _1, _2)
  );
}

void GameServer::on_register_failed(const Name& prefix, const std::string& reason) {
  std::cerr << "ERROR: Failed to register prefix '"
    << prefix << "' with the local forwarder (" << reason << ")"
    << std::endl;
  face.shutdown();
  running = 0;
}

void GameServer::join_interest(const InterestFilter&, const Interest& interest) {
  spdlog::info("new client ({}) is connected", connected_players);
  auto data = make_shared<Data>(interest.getName());
  data->setFreshnessPeriod(0);
  Stream content = {true};
  // send client_index to client
  Serialize::int_(content, &connected_players, 0, MAX_PLAYERS);
  data->setContent(content.data(), content.data.size());

  keychain.sign(*data);
  face.put(*data);
  connected_players += 1;
  interest_checklist.push_back({false, false, false});
}

void GameServer::client_disconnected(int client_index) {
  spdlog::warn("client {} disconnected", client_index);
  connected_players -= 1;
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
    update();
  }
  stop();
}

void send_interests() {
  // need to receive: newplayermessage, atepellet, playerupdate
  auto recv = [
    (int)GameMessageType::NEW_PLAYER,
    (int)GameMessageType::ATE_PELLET,
    (int)GameMessageType::PLAYER_UPDATE
  ];
  for(int id = 0; id < connected_players; id++) {
    for(int recm = 0; recm < 3; recm++) {
      int message = recv[recm];
      if(!interest_checklist[id][message]) {
        // hack
        Name interestName("/agario/client/");
        interestName.append(std::to_string(id));
        interestName.append(std::to_string(message));

        Interest interest(interestName);
        interest.setCanBePrefix(false);
        interest.setMustBeFresh(true);
        face.expressInterest(
          interest,
          bind(&GameServer::process_message, this,  _1, _2),
          [](const Interest& interest, const lp::Nack& nack) {
            flog->error("Received Nack with reason {}", nack.getReason());
            int message_id = std::stoi(interest.getName().at(-1));
            int client_id = std::stoi(interest.getName().at(-2));
            interest_checklist[client_id][message_id];
          },
          [](const Interest& interest) {
            flog->debug("Timeout for {}", interest);
            int message_id = std::stoi(interest.getName().at(-1));
            int client_id = std::stoi(interest.getName().at(-2));
            interest_checklist[client_id][message_id];
          }
        );
      }
    }
  }
}

void GameServer::update() {
  // stop server if all players finished the game
  if (game_started && connected_players == 0) {
    running = 0;
    return;
  }
  send_interests();
  face.processEvents();
  check_for_winners();
}

void GameServer::process_message(const Interest& interest, const Data& data) {
  int message_id = std::stoi(interest.getName().at(-1));
  int client_index = std::stoi(interest.getName().at(-2));
  Stream stream = {false};
  stream.read(data.getContent().value(), data.getContent().value_size());
  switch (message_id) {
  case (int)GameMessageType::NEW_PLAYER:
    NewPlayerMessage* message = new NewPlayerMessage();
    message.serialize(stream);
    process_newplayer_message(client_index, (NewPlayerMessage *)message);
    delete message;
    break;
  case (int)GameMessageType::ATE_PELLET:
    AtePelletMessage* message = new AtePelletMessage();
    message.serialize(stream);
    process_atepellet_message(client_index, (AtePelletMessage *)message);
    delete message;
    break;
  case (int)GameMessageType::PLAYER_UPDATE:
    PlayerUpdateMessage* message = new PlayerUpdateMessage();
    message.serialize(stream);
    process_playerupdate_message(client_index, (PlayerUpdateMessage *)message);
    delete message;
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

  // send game info to all players if lobby's filled
  if (connected_players == lobby_capacity) {
    game_started = true;
  }
  if (game_started) {
    for (int client_id = 0; client_id < connected_players; client_id++) {
      if (server.IsClientConnected(client_id)) {
        GameInfoMessage *reply = (GameInfoMessage *)server.CreateMessage(
            client_id, (int)GameMessageType::GAME_INFO);
        reply->player_index = client_id;
        reply->players = std::move(state.get_players());
        reply->pellets = state.pellets;
        reply->viruses = state.viruses;
        server.SendMessage(client_id, (int)GameChannel::RELIABLE, reply);
        spdlog::info("sending game info");
      }
    }
  }

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
    if (game_started) {
      SnapshotMessage *reply = (SnapshotMessage *)server.CreateMessage(
          client_index, (int)GameMessageType::SNAPSHOT);
      reply->id = ++snapshot_id;
      reply->stats = state.players_stats;
      server.SendMessage(client_index, (int)GameChannel::UNRELIABLE, reply);
      spdlog::debug("sent snapshot to {}", client_index);
    }

  } else {
    spdlog::debug("received older player update message of client {}",
                  client_index);
  }
}

void GameServer::check_for_winners() {
  if (game_started) {
    for (int id = 0; id < connected_players; id++) {
      if (state.players_stats[id].cells.size() != 0 &&
          std::get<2>(state.players_stats[id].cells[0]) > 200) {
        GameOverMessage *reply = (GameOverMessage *)server.CreateMessage(
            id, (int)GameMessageType::GAME_OVER);
        reply->rank = ++finished;
        server.SendMessage(id, (int)GameChannel::RELIABLE, reply);
      }
    }
  }
}

void GameServer::stop() {
  join_handler.unregister();
  message_handler.unregister();
  face.shutdown();
  spdlog::critical("server is stopped.");
}
