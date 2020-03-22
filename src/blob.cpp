#include "blob.hpp"
#include "game_settings.hpp"
#include <cmath>

void Blob::follow_mouse(int mx, int my, Camera& camera) {
    float mouse_x = mx * camera.current_scale;
    float mouse_y = my * camera.current_scale;

    float v_x = mouse_x - (x - camera.x_offset());
    float v_y = mouse_y - (y - camera.y_offset());

    float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
    if(magnitude == 0)
        return;

    float vel_x = SPEED * v_x/magnitude;
    // move blob
    x += vel_x;
    // ensure boundaries
    if(x < radius) x = radius;
    if((x + radius) > PLAYGROUND_WIDTH) x = PLAYGROUND_WIDTH - radius;

    float vel_y = SPEED * v_y/magnitude;
    y += vel_y;
    if(y < radius) y = radius;
    if((y + radius) > PLAYGROUND_HEIGHT) y = PLAYGROUND_HEIGHT - radius;
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
