#include "game.hpp"

int main(int argc, char *argv[]) {
  Config config = ArgParse::parse_cmd(argc, argv);
  Game::run(std::move(config));
  return 0;
}
