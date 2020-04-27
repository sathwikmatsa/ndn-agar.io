#pragma once
#include "./../shared/game_settings.hpp"
#include "cell.hpp"
#include <SDL2/SDL.h>

struct Projectile {
  Cell cell;
  float direction_x;
  float direction_y;
  float speed;
  Uint32 last_t;
  float deceleration;

  Projectile(Cell blob, float dx, float dy, float s, float dec)
      : cell(blob), direction_x{dx}, direction_y{dy}, speed{s}, deceleration{
                                                                    dec} {
    last_t = SDL_GetTicks();
  }

  bool at_rest() { return speed == 0; }

  void update_pos() {
    Uint32 present_t = SDL_GetTicks();

    float new_speed = speed - deceleration * (present_t - last_t) / 400.0f;

    float distance = (new_speed + speed) * (present_t - last_t) / 800.0f;

    cell.x += distance * direction_x;
    cell.y += distance * direction_y;

    // ensure boundaries
    if (cell.x < EJECTILE_RADIUS)
      cell.x = EJECTILE_RADIUS;
    if ((cell.x + EJECTILE_RADIUS) > PLAYGROUND_WIDTH)
      cell.x = PLAYGROUND_WIDTH - EJECTILE_RADIUS;

    if (cell.y < EJECTILE_RADIUS)
      cell.y = EJECTILE_RADIUS;
    if ((cell.y + EJECTILE_RADIUS) > PLAYGROUND_HEIGHT)
      cell.y = PLAYGROUND_HEIGHT - EJECTILE_RADIUS;

    speed = new_speed;

    // zero acceleration when cell comes to rest
    if (speed <= 0) {
      speed = 0;
      deceleration = 0;
    }

    last_t = present_t;
  }
};
