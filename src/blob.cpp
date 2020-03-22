#include "blob.hpp"
#include "game_settings.hpp"
#include <cmath>

void Blob::follow_mouse(int x1, int y1, Context& ctx) {
    float v_x = x1 - (x - ctx.camera.x_offset());
    float v_y = y1 - (y - ctx.camera.y_offset());

    float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
    if(magnitude == 0)
        return;

    int vel_x = std::round(SPEED * v_x/magnitude);
    // move blob
    x += vel_x;
    if((x < radius) || ((x + radius) > PLAYGROUND_WIDTH)) {
        // move back
        x -= vel_x;
    }

    int vel_y = std::round(SPEED * v_y/magnitude);
    y += vel_y;
    if((y < radius) || ((y + radius) > PLAYGROUND_HEIGHT)) {
        y -= vel_y;
    }
}

float Blob::get_radius() {
    return radius;
}

bool Blob::can_eat(Blob &other_blob) {
    float r2 = other_blob.get_radius();
    float distance_centers = std::sqrt(std::pow(x - other_blob.x, 2)
            + std::pow(y - other_blob.y, 2));
    if(distance_centers + r2 < radius)
        return true;
    else
        return false;
}

void Blob::consume(Blob &other_blob) {
    float r2 = other_blob.get_radius();
    radius = std::sqrt(radius * radius + r2 * r2);
    other_blob.succumb();
}

void Blob::succumb() {
    radius = 0;
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
