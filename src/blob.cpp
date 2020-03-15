#include "blob.hpp"
#include "game_settings.hpp"
#include <cmath>

void Blob::translate(int i, int j) {
    x += i;
    y += j;
}

unsigned int Blob::get_radius() {
    return radius;
}

bool Blob::can_eat(Blob &other_blob) {
    unsigned int r2 = other_blob.get_radius();
    int distance_centers = static_cast<unsigned int>(
        std::sqrt(std::pow(x - other_blob.x, 2) + std::pow(y - other_blob.y, 2))
    );
    if(distance_centers + r2 < radius)
        return true;
    else
        return false;
}

void Blob::consume(Blob &other_blob) {
    unsigned int r2 = other_blob.get_radius();
    radius = static_cast<unsigned int>(std::sqrt(radius * radius + r2 * r2));
}

Blob::Blob(int x1, int y1) :
    x(x1),
    y(y1),
    radius(STATIC_BLOB_RADIUS),
    is_npc(true)
{
}

Blob::Blob(int x1, int y1, std::string name) :
    x(x1),
    y(y1),
    radius(PLAYER_BLOB_RADIUS),
    is_npc(false),
    player_name(name)
{
}
