#include "pellet.hpp"
#include <random>
#include "game_settings.hpp"

float Pellet::get_size() {
    return radius/2;
}

void Pellet::succumb() {
    radius = 0;
}

Pellet::Pellet(int pos_x, int pos_y) {
    radius = PELLET_RADIUS;
    x = pos_x;
    y = pos_y;

    std::random_device rd;
    std::mt19937 eng(rd());

    // random color
    std::uniform_int_distribution<> distrc(0, 255);
    r = distrc(eng);
    g = distrc(eng);
    b = distrc(eng);
}
