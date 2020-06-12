#pragma once

#include "./../shared/game_settings.hpp"
#include "./../shared/stream.hpp"
#include "game_state.hpp"
#include <spdlog/spdlog.h>
#include <yojimbo/yojimbo.h>
#include <ndn-cxx/name.hpp>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

class GameServer {
public:
  GameServer(ndn::Name prefix, int lobby_capacity);
  void run();
  void stop();

private:
  GameState state;
  void update(float dt);
  void check_for_winners();
  void process_messages();
  void process_message(int client_index);
  void process_newplayer_message(int client_index, NewPlayerMessage *message);
  void process_atepellet_message(int client_index, AtePelletMessage *message);
  void process_playerupdate_message(int client_index,
                                    PlayerUpdateMessage *message);
  void client_connected(int client_index);
  void client_disconnected(int client_index);
  void send_data(Stream& stream, ndn::time::milliseconds freshness_period);
  std::shared_ptr<spdlog::logger> flog;
  float time;
  uint32_t snapshot_id;
  int lobby_capacity;
  int connected_players;
  int finished;
  bool game_started;

  ndn::Face face;
  ndn::KeyChain keychain;
};
