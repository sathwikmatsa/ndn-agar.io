#include "agar.hpp"
#include "pellet.hpp"
#include "game_settings.hpp"
#include <cmath>
#include <random>
#include "projectile.hpp"
#include <optional>

void Agar::follow_mouse(int mx, int my, Camera& camera) {
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

std::optional<std::unique_ptr<Projectile>>
Agar::eject(int mx, int my, Camera& camera) {
    float mouse_x = mx * camera.current_scale;
    float mouse_y = my * camera.current_scale;

    float v_x = mouse_x - (x - camera.x_offset());
    float v_y = mouse_y - (y - camera.y_offset());

    float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
    if(magnitude == 0 || radius < 50)
        return std::nullopt;

    float dx = v_x / magnitude;
    float dy = v_y / magnitude;

    std::unique_ptr<Pellet> eject_cell(
        new Pellet(
            x + radius * dx,
            y + radius * dy,
            r,
            g,
            b
        )
    );
    std::unique_ptr<Projectile> projectile(new
        Projectile(
            std::move(eject_cell),
            dx,
            dy,
            EJECTILE_INIT_VELOCITY,
            EJECTILE_DECELERATION
        )
    );

    // reduce radius of player
    radius = std::sqrt(radius * radius - EJECTILE_RADIUS * EJECTILE_RADIUS);
    std::optional<std::unique_ptr<Projectile>> value{std::move(projectile)};

    return value;
}

bool Agar::can_eat(Cell& other_cell) {
    float r2 = other_cell.get_size();
    float distance_centers = std::sqrt(
        std::pow(x - other_cell.x, 2) + std::pow(y - other_cell.y, 2)
    );
    if(distance_centers + r2 < radius)
        return true;
    else
        return false;
}

void Agar::consume(Cell& other_cell) {
    float r2 = other_cell.get_size();
    radius = std::sqrt(radius * radius + r2 * r2);
    other_cell.succumb();
}

void Agar::succumb() {
    radius = 0;
}

float Agar::get_size() {
    return radius;
}

Agar::Agar(std::string name) :
    player_name(name)
{
    radius = AGAR_RADIUS;
    std::random_device rd;
    std::mt19937 eng(rd());

    // random x pos
    std::uniform_int_distribution<> distposx(radius, PLAYGROUND_WIDTH - radius);
    x = distposx(eng);

    // random y pos
    std::uniform_int_distribution<> distposy(radius, PLAYGROUND_HEIGHT - radius);
    y = distposy(eng);

    // random color
    std::uniform_int_distribution<> distrc(0, 255);
    r = distrc(eng);
    g = distrc(eng);
    b = distrc(eng);
}
