#pragma once

#include "../common/IServerConnection.hpp"
#include "../common/game_adapter.hpp"
#include "../common/game_connection_config.hpp"
#include "game_state.hpp"
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
  GameConnectionConfig conn_config;
  GameAdapter adapter;
  yojimbo::Server server;
  int pellets_eaten;
  float time;
  bool running;
};
