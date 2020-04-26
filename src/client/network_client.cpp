#include "network_client.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <yojimbo/yojimbo.h>

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = {0};

NetworkClient::NetworkClient(const yojimbo::Address &server_address)
    : client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"),
             conn_config, adapter, 0.0) {
  uint64_t clientId;
  yojimbo::random_bytes((uint8_t *)&clientId, 8);
  client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, server_address);
  running = true;
  update_id = 0;
  flog = spdlog::get("flog");
  last_snapshot_id = 0;
}

void NetworkClient::update(float deltat, World &world) {
  // update client
  client.AdvanceTime(client.GetTime() + deltat);
  client.ReceivePackets();

  if (client.IsConnected()) {
    process_messages(world);
    send_playerupdate(world);
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
  case (int)GameMessageType::NEW_PLAYER:
    process_newplayer_message((NewPlayerMessage *)message, world);
    break;
  case (int)GameMessageType::GAME_INFO:
    process_gameinfo_message((GameInfoMessage *)message, world);
    break;
  case (int)GameMessageType::PELLET_RELOC:
    process_pelletreloc_message((PelletRelocMessage *)message, world);
    break;
  case (int)GameMessageType::SNAPSHOT:
    process_snapshot_message((SnapshotMessage *)message, world);
    break;
  case (int)GameMessageType::DEAD_PLAYER:
    process_deadplayer_message((DeadPlayerMessage *)message, world);
    break;
  case (int)GameMessageType::GAME_OVER:
    process_gameover_message((GameOverMessage *)message, world);
    break;
  default:
    std::cout << "unexpected message\n" << std::endl;
    break;
  }
}

void NetworkClient::process_newplayer_message(NewPlayerMessage *message,
                                              World &world) {
  spdlog::info("received new player message");
  int p_index = message->player_index;
  int p_info_size = world.players_info.size();
  if (p_info_size <= p_index) {
    spdlog::debug("inserting opponent in new slot {}", p_index);
    world.players_info.resize(p_index + 1);
  } else {
    spdlog::debug("using pre existing slot for opponent {}", p_index);
  }
  world.players_info[p_index] =
      std::make_tuple(true, std::string(message->player_name), message->r,
                      message->g, message->b);
}

void NetworkClient::process_gameinfo_message(GameInfoMessage *message,
                                             World &world) {
  spdlog::debug("received game info message");
  world.my_index = message->player_index;
  auto players = message->players;
  auto pellets = message->pellets;
  auto viruses = message->viruses;

  for (auto &player : players) {
    world.add_player(player);
  }

  for (auto &pellet : pellets) {
    world.create_pellet(pellet);
  }

  for (auto &virus : viruses) {
    world.create_virus(virus);
  }
}

void NetworkClient::process_pelletreloc_message(PelletRelocMessage *message,
                                                World &world) {
  spdlog::debug("received pellet reloc message");
  int pellet_id = message->pellet_id;
  int x = message->pos_x;
  int y = message->pos_y;

  world.relocate_pellet(pellet_id, x, y);
}

void NetworkClient::process_gameover_message(GameOverMessage *message,
                                             World &world) {
  spdlog::warn("received game over message");
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
  spdlog::info("requested to join game");
}

void NetworkClient::send_atepellet_message(int id) {
  spdlog::debug("sent ate pellet message");
  AtePelletMessage *message = (AtePelletMessage *)client.CreateMessage(
      (int)GameMessageType::ATE_PELLET);
  message->pellet_id = id;
  client.SendMessage((int)GameChannel::RELIABLE, message);
}

void NetworkClient::send_playerupdate(World &world) {
  spdlog::debug("sent player update message");
  PlayerStats info = (*world.agar).get_player_stats();
  PlayerUpdateMessage *message = (PlayerUpdateMessage *)client.CreateMessage(
      (int)GameMessageType::PLAYER_UPDATE);
  message->seq_id = ++update_id;
  message->info = std::move(info);
  client.SendMessage((int)GameChannel::UNRELIABLE, message);
}

void NetworkClient::process_snapshot_message(SnapshotMessage *message,
                                             World &world) {
  if (message->id > last_snapshot_id) {
    world.players_stats = std::move(message->stats);
    last_snapshot_id = message->id;
    spdlog::debug("received snapshot message");
  } else {
    spdlog::debug("received old snapshot");
  }
}

void NetworkClient::process_deadplayer_message(DeadPlayerMessage *message,
                                               World &world) {
  spdlog::warn("received dead player message for {}", message->player_index);
  int n_players = world.players_info.size();
  if (n_players > message->player_index) {
    std::get<0>(world.players_info[message->player_index]) = false;
    flog->warn("deactivated player {}",
               std::get<1>(world.players_info[message->player_index]));
  }
}

void NetworkClient::close_connection() {
  client.Disconnect();
  client.~Client();
}
