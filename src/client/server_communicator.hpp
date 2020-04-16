#pragma once

#include "../common/IServerConnection.hpp"
#include "../common/game_adapter.hpp"
#include "../common/game_connection_config.hpp"
#include <yojimbo/yojimbo.h>

class ServerCommunicator {
  public:
    ServerCommunicator(const yojimbo::Address &server_address);
    void run();
    bool sent_message;

  private:
    void update(float dt);
    void process_messages();
    void process_message(yojimbo::Message *message);
    void process_newplayer_message(NewPlayerMessage *message);
    GameConnectionConfig conn_config;
    GameAdapter adapter;
    bool running;
    float time;

  public:
    yojimbo::Client client;
};
