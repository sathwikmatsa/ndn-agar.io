#include "agar.hpp"
#include "game_settings.hpp"
#include <cmath>
#include <random>
#include "projectile.hpp"
#include <tuple>
#include <vector>

void Agar::follow_mouse(int mx, int my, Camera& camera) {
    float mouse_x = mx * camera.current_scale;
    float mouse_y = my * camera.current_scale;

    for(auto &cell: cells) {
        float v_x = mouse_x - (cell.x - camera.x_offset());
        float v_y = mouse_y - (cell.y - camera.y_offset());

        float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
        if(magnitude == 0)
            return;

        float vel_x = SPEED * v_x/magnitude;
        // move blob
        cell.x += vel_x;
        // ensure boundaries
        if(cell.x < cell.radius) cell.x = cell.radius;
        if((cell.x + cell.radius) > PLAYGROUND_WIDTH)
            cell.x = PLAYGROUND_WIDTH - cell.radius;

        float vel_y = SPEED * v_y/magnitude;
        cell.y += vel_y;
        if(cell.y < cell.radius)
            cell.y = cell.radius;
        if((cell.y + cell.radius) > PLAYGROUND_HEIGHT)
            cell.y = PLAYGROUND_HEIGHT - cell.radius;
    }
}

float Agar::get_size() {
    float max = 0.0;
    for(auto &cell: cells) {
        if(cell.radius > max)
            max = cell.radius;
    }
    return max;
}

std::vector<Projectile> Agar::eject(int mx, int my, Camera& camera) {
    float mouse_x = mx * camera.current_scale;
    float mouse_y = my * camera.current_scale;

    std::vector<Projectile> ejectiles;
    for(auto &cell: cells) {
        float v_x = mouse_x - (cell.x - camera.x_offset());
        float v_y = mouse_y - (cell.y - camera.y_offset());

        float magnitude = std::sqrt(v_x * v_x + v_y * v_y);
        if(magnitude == 0 || cell.radius < 50)
            continue;

        float dx = v_x / magnitude;
        float dy = v_y / magnitude;

        Cell eject_cell(
            {
                CellType::Ejectile,
                int(cell.x + cell.radius * dx),
                int(cell.y + cell.radius * dy),
                r,
                g,
                b,
                EJECTILE_RADIUS
            }
        );
        Projectile projectile(
            std::move(eject_cell),
            dx,
            dy,
            EJECTILE_INIT_VELOCITY,
            EJECTILE_DECELERATION
        );

        // reduce radius of player
        cell.radius = std::sqrt(cell.radius * cell.radius - EJECTILE_RADIUS * EJECTILE_RADIUS);
        ejectiles.push_back(std::move(projectile));
    }
    return ejectiles;
}

void Agar::render(Context& ctx) {
    for(auto &cell: cells) {
        ctx.txt->render(cell, ctx.camera, ctx.renderer);
    }
}

std::tuple<int, int> Agar::get_center() {
    float sum_x = 0;
    float sum_y = 0;
    int n = 0;
    for(auto &cell: cells) {
        sum_x += cell.x;
        sum_y += cell.y;
        n += 1;
    }
    return std::make_tuple(sum_x/n, sum_y/n);
}

Agar::Agar(std::string name, CellSettings cs) :
    r(cs.r), g(cs.g), b(cs.b),
    player_name(name)
{
    cells.push_back(Cell(cs));
}
