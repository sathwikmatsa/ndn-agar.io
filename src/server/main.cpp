#include "game_server.hpp"
#include <iostream>
#include <stdexcept>
#include <yojimbo/yojimbo.h>
#include "./../shared/logger_manager.hpp"

int main() {
  LoggerManager::setup("server");
  if (InitializeYojimbo()) {
    yojimbo::Address address(127, 0, 0, 1, 9999);
    GameServer server(address);
    server.run();
    ShutdownYojimbo();
  } else {
    throw std::runtime_error("Failed to initialize yojimbo!");
  }
  return 0;
}
