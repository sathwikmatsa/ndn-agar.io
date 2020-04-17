#pragma once

#include "../common/IServerConnection.hpp"
#include "../common/game_adapter.hpp"
#include "../common/game_connection_config.hpp"
#include <yojimbo/yojimbo.h>

class NetworkClient {
  public:
    NetworkClient(const yojimbo::Address &server_address);
    void run();
    bool sent_info;

  private:
    void update(float dt);
    void process_messages();
    void process_message(yojimbo::Message *message);
    void process_pelletsinfo_message(PelletsInfoMessage *message);
    void process_pelletreloc_message(PelletRelocMessage *message);
    void process_gameover_message(GameOverMessage *message);
    GameConnectionConfig conn_config;
    GameAdapter adapter;
    bool running;
    float time;

  public:
    yojimbo::Client client;
};
