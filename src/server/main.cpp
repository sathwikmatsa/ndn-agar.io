#include "./../shared/logger_manager.hpp"
#include "game_server.hpp"
#include <iostream>
#include <stdexcept>
#include <yojimbo/yojimbo.h>

static const int DEFAULT_N_PLAYERS = 5;

int main(int argc, char *argv[]) {
  LoggerManager::setup("server");
  if (InitializeYojimbo()) {
    int n_players = DEFAULT_N_PLAYERS;
    if (argc == 2) {
      n_players = std::stoi(argv[1]);
    }
    yojimbo::Address address(127, 0, 0, 1, 9999);
    GameServer server(address, n_players);
    server.run();
    ShutdownYojimbo();
  } else {
    throw std::runtime_error("Failed to initialize yojimbo!");
  }
  return 0;
}
