#pragma once
#include <SDL2/SDL.h>
#include "cell.hpp"

struct Projectile {
    std::unique_ptr<Cell> cell;
    float direction_x;
    float direction_y;
    float speed;
    Uint32 last_t;
    float deceleration;

    Projectile(std::unique_ptr<Cell> blob, float dx, float dy, float s, float dec) :
        direction_x{dx},
        direction_y{dy},
        speed {s},
        deceleration {dec}
    {
        cell = std::move(blob);
        last_t = SDL_GetTicks();
    }

    bool at_rest() {
        return speed == 0;
    }

    void update_pos() {
        Uint32 present_t = SDL_GetTicks();

        float new_speed = speed - deceleration * (present_t - last_t) / 400.0f;

        float distance = (new_speed + speed) * (present_t - last_t) / 800.0f;

        (*cell).x += distance * direction_x;
        (*cell).y += distance * direction_y;

        speed = new_speed;

        // zero acceleration when cell comes to rest
        if(speed <= 0){
            speed = 0;
            deceleration = 0;
        }

        last_t = present_t;
    }
};
