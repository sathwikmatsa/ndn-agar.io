#include <yojimbo/yojimbo.h>
#include "game_server.hpp"

int main() {
    yojimbo::Address address(127, 0, 0, 1, 9999);
    GameServer server(address);
    return 0;
}
