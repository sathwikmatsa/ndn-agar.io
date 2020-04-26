#pragma once

#include "../common/IServerConnection.hpp"
#include "../common/game_adapter.hpp"
#include "../common/game_connection_config.hpp"
#include "world.hpp"
#include <spdlog/spdlog.h>
#include <string>
#include <yojimbo/yojimbo.h>

class NetworkClient {
public:
  NetworkClient(const yojimbo::Address &server_address);
  void update(float dt, World &world);
  void join_room(std::string player_name);
  void send_atepellet_message(int id);
  void close_connection();

private:
  void process_messages(World &world);
  void process_message(yojimbo::Message *message, World &world);
  void process_newplayer_message(NewPlayerMessage *message, World &world);
  void process_gameinfo_message(GameInfoMessage *message, World &world);
  void process_pelletreloc_message(PelletRelocMessage *message, World &world);
  void process_snapshot_message(SnapshotMessage *message, World &world);
  void process_deadplayer_message(DeadPlayerMessage *message, World &world);
  void process_gameover_message(GameOverMessage *message, World &world);
  void send_playerupdate(World &world);
  GameConnectionConfig conn_config;
  GameAdapter adapter;
  bool running;
  float time;
  uint32_t update_id;
  uint32_t last_snapshot_id;
  std::shared_ptr<spdlog::logger> flog;

public:
  yojimbo::Client client;
};
