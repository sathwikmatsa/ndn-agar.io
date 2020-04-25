#include "game_server.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <yojimbo/yojimbo.h>

int main() {
#ifdef DEBUG
#define LOG_FILEPATH "debug/server_log_" __TIME__ ".txt"
  spdlog::set_level(spdlog::level::debug);
#else
#define LOG_FILEPATH "server_log_" __TIME__ ".txt"
  spdlog::set_level(spdlog::level::info);
#endif
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
