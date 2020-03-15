#include "blob.hpp"
#include "game_settings.hpp"
#include <cmath>
#include <iostream>

void Blob::follow_mouse(int x1, int y1) {
    float v_x = x1 - x;
    float v_y = y1 - y;
    float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
    if(magnitude == 0)
        return;
    x += std::round(SPEED * v_x/magnitude);
    y += std::round(SPEED * v_y/magnitude);
}

int Blob::get_radius() {
    return radius;
}

bool Blob::can_eat(Blob &other_blob) {
    int r2 = other_blob.get_radius();
    int distance_centers = static_cast<int>(
        std::sqrt(std::pow(x - other_blob.x, 2) + std::pow(y - other_blob.y, 2))
    );
    if(distance_centers + r2 < radius)
        return true;
    else
        return false;
}

void Blob::consume(Blob &other_blob) {
    int r2 = other_blob.get_radius();
    radius = static_cast<int>(std::sqrt(radius * radius + r2 * r2));
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
