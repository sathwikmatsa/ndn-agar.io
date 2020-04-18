#include "network_client.hpp"
#include <iostream>
#include <yojimbo/yojimbo.h>

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = {0};

NetworkClient::NetworkClient(const yojimbo::Address &server_address)
    : client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"),
             conn_config, adapter, 0.0) {
  uint64_t clientId;
  yojimbo::random_bytes((uint8_t *)&clientId, 8);
  client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, server_address);
  running = true;
}

void NetworkClient::update(float deltat, World &world) {
  // update client
  client.AdvanceTime(client.GetTime() + deltat);
  client.ReceivePackets();

  if (client.IsConnected()) {
    process_messages(world);
  }

  client.SendPackets();
}

void NetworkClient::process_messages(World &world) {
  for (int i = 0; i < conn_config.numChannels; i++) {
    yojimbo::Message *message = client.ReceiveMessage(i);
    while (message != NULL) {
      process_message(message, world);
      client.ReleaseMessage(message);
      message = client.ReceiveMessage(i);
    }
  }
}

void NetworkClient::process_message(yojimbo::Message *message, World &world) {
  switch (message->GetType()) {
  case (int)GameMessageType::NPC_INFO:
    process_npcinfo_message((NpcInfoMessage *)message, world);
    break;
  case (int)GameMessageType::PELLET_RELOC:
    process_pelletreloc_message((PelletRelocMessage *)message, world);
    break;
  case (int)GameMessageType::GAME_OVER:
    process_gameover_message((GameOverMessage *)message, world);
    break;
  default:
    std::cout << "unexpected message\n" << std::endl;
    break;
  }
}

void NetworkClient::process_npcinfo_message(NpcInfoMessage *message,
                                            World &world) {
  auto pellets = message->pellets;
  auto viruses = message->viruses;

  for (auto &pellet : pellets) {
    world.create_pellet(pellet);
  }

  for (auto &virus : viruses) {
    world.create_virus(virus);
  }
}

void NetworkClient::process_pelletreloc_message(PelletRelocMessage *message,
                                                World &world) {
  int pellet_id = message->pellet_id;
  int x = message->pos_x;
  int y = message->pos_y;

  world.relocate_pellet(pellet_id, x, y);
}

void NetworkClient::process_gameover_message(GameOverMessage *message,
                                             World &world) {
  world.running = !(message->gameover);
}

void NetworkClient::join_room(std::string player_name) {
  NewPlayerMessage *message = (NewPlayerMessage *)client.CreateMessage(
      (int)GameMessageType::NEW_PLAYER);
  message->r = 0;
  message->g = 0;
  message->b = 0;
  strcpy(message->player_name, player_name.c_str());
  client.SendMessage((int)GameChannel::RELIABLE, message);
}

void NetworkClient::send_atepellet_message(int id) {
  AtePelletMessage *message = (AtePelletMessage *)client.CreateMessage(
      (int)GameMessageType::ATE_PELLET);
  message->pellet_id = id;
  client.SendMessage((int)GameChannel::RELIABLE, message);
}

void NetworkClient::close_connection() {
  client.Disconnect();
  client.~Client();
}
