#pragma once

#include "./../shared/data_sync.hpp"
#include "./../shared/game_settings.hpp"
#include "./../shared/stream.hpp"
#include "game_state.hpp"
#include <spdlog/spdlog.h>

class GameServer {
public:
  GameServer(int lobby_capacity);
  void run();
  void stop();

private:
  GameState state;
  void update();
  void check_for_winners();
  void process_messages();
  void process_message(const ndn::Interest &interest, const ndn::Data &data);
  void process_newplayer_message(int client_index, NewPlayerMessage *message);
  void process_atepellet_message(int client_index, AtePelletMessage *message);
  void process_playerupdate_message(int client_index,
                                    PlayerUpdateMessage *message);
  void client_connected(int client_index);
  void client_disconnected(int client_index);

  void join_interest(const ndn::InterestFilter &,
                     const ndn::Interest &interest);
  void on_register_failed(const ndn::Name &prefix, const std::string &reason);
  void register_listeners(int client_id);
  std::shared_ptr<spdlog::logger> flog;
  float time;
  uint32_t snapshot_id;
  int lobby_capacity;
  int connected_players;
  int finished;
  bool game_started;

  DataSync sync;
};
