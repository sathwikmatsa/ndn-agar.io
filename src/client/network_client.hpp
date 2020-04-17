#pragma once

#include "../common/IServerConnection.hpp"
#include "../common/game_adapter.hpp"
#include "../common/game_connection_config.hpp"
#include "world.hpp"
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
    void process_npcinfo_message(NpcInfoMessage *message, World &world);
    void process_pelletreloc_message(PelletRelocMessage *message, World &world);
    void process_gameover_message(GameOverMessage *message, World &world);
    GameConnectionConfig conn_config;
    GameAdapter adapter;
    bool running;
    float time;

  public:
    yojimbo::Client client;
};
