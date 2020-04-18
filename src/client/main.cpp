#include "game.hpp"

int main(int argc, char *argv[]) {
    Config config = ArgParse::parse_cmd(argc, argv);
    Game::run(config);
    return 0;
}
