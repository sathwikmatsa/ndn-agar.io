#include "game_server.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <yojimbo/yojimbo.h>

// null private key
static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = {0};

static const int MAX_PLAYERS = 16;

GameServer::GameServer(const yojimbo::Address &address)
    : adapter(this), server(yojimbo::GetDefaultAllocator(), DEFAULT_PRIVATE_KEY,
                            address, conn_config, adapter, 0.0f) {
  server.Start(MAX_PLAYERS);
  if (!server.IsRunning()) {
    throw std::runtime_error("Could not start server at port " +
                             std::to_string(address.GetPort()));
  } else {
    running = true;
  }
  // print the port we got in case we used port 0
  char buffer[256];
  server.GetAddress().ToString(buffer, sizeof(buffer));
  std::cout << "Server is running at address: " << buffer << std::endl;
  pellets_eaten = 0;
}

void GameServer::client_connected(int client_index) {
  std::cout << "client " << client_index << " connected" << std::endl;
}

void GameServer::client_disconnected(int client_index) {
  std::cout << "client " << client_index << " disconnected" << std::endl;
}

void GameServer::run() {
  float fixed_dt = 1.0f / 60.0f;
  while (running) {
    double current_time = yojimbo_time();
    if (time <= current_time) {
      update(fixed_dt);
      time += fixed_dt;
    } else {
      yojimbo_sleep(time - current_time);
    }
  }
  server.Stop();
}

void GameServer::update(float deltat) {
  // stop if server is not running
  if (!server.IsRunning()) {
    running = false;
    return;
  }

  // update server and process messages
  server.AdvanceTime(server.GetTime() + deltat);
  server.ReceivePackets();
  process_messages();

  // ... process client inputs ...
  // ... update game ...
  // ... send game state to clients ...
  if (pellets_eaten == 10) {
    GameOverMessage *reply = (GameOverMessage *)server.CreateMessage(
        0, (int)GameMessageType::GAME_OVER);
    reply->gameover = true;
    server.SendMessage(0, (int)GameChannel::RELIABLE, reply);
    std::cout << "Sent GameOverMessage" << std::endl;
    pellets_eaten = 0;
  }

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
  default:
    std::cout << "unexpected message" << std::endl;
    break;
  }
}

void GameServer::process_newplayer_message(int client_index,
                                           NewPlayerMessage *message) {
  state.add_player(std::string(message->player_name));
  if (server.IsClientConnected(client_index)) {
    NpcInfoMessage *reply = (NpcInfoMessage *)server.CreateMessage(
        client_index, (int)GameMessageType::NPC_INFO);
    reply->pellets = state.pellets;
    reply->viruses = state.viruses;
    server.SendMessage(client_index, (int)GameChannel::RELIABLE, reply);
  }
}

void GameServer::process_atepellet_message(int client_index,
                                           AtePelletMessage *message) {
  auto [new_x, new_y] = state.relocate_pellet(message->pellet_id);
  if (server.IsClientConnected(client_index)) {
    PelletRelocMessage *reply = (PelletRelocMessage *)server.CreateMessage(
        client_index, (int)GameMessageType::PELLET_RELOC);
    reply->pellet_id = message->pellet_id;
    reply->pos_x = new_x;
    reply->pos_y = new_y;
    server.SendMessage(client_index, (int)GameChannel::RELIABLE, reply);
  }
  pellets_eaten += 1;
}

void GameServer::stop() { server.Stop(); }
