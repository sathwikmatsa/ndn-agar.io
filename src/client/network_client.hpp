#pragma once

#include "./../shared/data_sync.hpp"
#include "./../shared/game_settings.hpp"
#include "world.hpp"
#include <spdlog/spdlog.h>
#include <string>

class NetworkClient {
public:
  NetworkClient();
  void update(World &world);
  void connect_to_server(std::string player_name, World &world);
  void send_atepellet_message(int id);
  void close_connection();

private:
  void join_room(const ndn::Interest &, const ndn::Data &data,
                 std::string player_name, World &world);
  void process_message(const ndn::Interest &interest, const ndn::Data &data,
                       World &world);
  void process_newplayer_message(NewPlayerMessage *message, World &world);
  void process_gameinfo_message(GameInfoMessage *message, World &world);
  void process_pelletreloc_message(PelletRelocMessage *message, World &world);
  void process_snapshot_message(SnapshotMessage *message, World &world);
  void process_deadplayer_message(DeadPlayerMessage *message, World &world);
  void process_gameover_message(GameOverMessage *message, World &world);
  void send_playerupdate(World &world);
  void on_register_failed(const ndn::Name &prefix, const std::string &reason);
  bool running;
  float time;
  uint32_t update_id;
  uint32_t last_snapshot_id;
  std::shared_ptr<spdlog::logger> flog;
  DataSync sync;
};
