#include "./../shared/logger_manager.hpp"
#include "argparse.hpp"
#include "game_server.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  LoggerManager::setup("server");
  Config config = ArgParse::parse_cmd(argc, argv);
  GameServer server(config.lobby_capacity);
  server.run();
  return 0;
}
