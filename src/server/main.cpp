#include "./../shared/logger_manager.hpp"
#include "game_server.hpp"
#include "argparse.hpp"

int main(int argc, char *argv[]) {
  LoggerManager::setup("server");
  Config config = ArgParse::parse_cmd(argc, argv);
  std::cout << config.server_prefix << std::endl;
  std::cout << config.lobby_capacity << std::endl;
  GameServer server(config);
  server.run();
  return 0;
}
