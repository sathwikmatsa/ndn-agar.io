#include "game.hpp"
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
#ifdef DEBUG
  spdlog::set_level(spdlog::level::debug);
#else
  spdlog::set_level(spdlog::level::info);
#endif
  Config config = ArgParse::parse_cmd(argc, argv);
  Game::run(std::move(config));
  return 0;
}
