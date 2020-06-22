#pragma once

#include "./../shared/data_sync.hpp"
#include "./../shared/game_settings.hpp"
#include "world.hpp"
#include <spdlog/spdlog.h>
#include <string>

class NetworkClient {
public:
  NetworkClient(World *w);
  void update();
  void connect_to_server(std::string player_name);
  void send_atepellet_message(int id);
  void close_connection();

private:
  void join_room(const ndn::Interest &, const ndn::Data &data,
                 std::string player_name);
  void process_message(const ndn::Interest &interest, const ndn::Data &data);
  void process_newplayer_message(NewPlayerMessage *message);
  void process_gameinfo_message(GameInfoMessage *message);
  void process_pelletreloc_message(PelletRelocMessage *message);
  void process_snapshot_message(SnapshotMessage *message);
  void process_deadplayer_message(DeadPlayerMessage *message);
  void process_gameover_message(GameOverMessage *message);
  void send_playerupdate();
  void on_register_failed(const ndn::Name &prefix, const std::string &reason);
  bool running;
  float time;
  uint32_t update_id;
  uint32_t last_snapshot_id;
  std::shared_ptr<spdlog::logger> flog;
  DataSync sync;
  World *world;
};
