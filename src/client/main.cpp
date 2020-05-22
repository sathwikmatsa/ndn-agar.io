#include "game.hpp"
#include "./../shared/logger_manager.hpp"

int main(int argc, char *argv[]) {
  LoggerManager::setup("client");
  Config config = ArgParse::parse_cmd(argc, argv);
  Game::run(std::move(config));
  return 0;
}
