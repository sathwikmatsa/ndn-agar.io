#pragma once

#include "../shared/IServerConnection.hpp"
#include "../shared/game_adapter.hpp"
#include "../shared/game_connection_config.hpp"
#include "game_state.hpp"
#include <spdlog/spdlog.h>
#include <yojimbo/yojimbo.h>

class GameServer : public IServerConnection {
public:
  GameServer(const yojimbo::Address &address);
  void client_connected(int client_index) override;
  void client_disconnected(int client_index) override;
  void run();
  void stop();

private:
  GameState state;
  void update(float dt);
  void process_messages();
  void process_message(int client_index, yojimbo::Message *message);
  void process_newplayer_message(int client_index, NewPlayerMessage *message);
  void process_atepellet_message(int client_index, AtePelletMessage *message);
  void process_playerupdate_message(int client_index,
                                    PlayerUpdateMessage *message);
  GameConnectionConfig conn_config;
  GameAdapter adapter;
  yojimbo::Server server;
  std::shared_ptr<spdlog::logger> flog;
  float time;
  uint32_t snapshot_id;
};
