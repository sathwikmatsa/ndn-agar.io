#include "game.hpp"
#include "logger_manager.hpp"

int main(int argc, char *argv[]) {
  LoggerManager::setup();
  Config config = ArgParse::parse_cmd(argc, argv);
  Game::run(std::move(config));
  return 0;
}
