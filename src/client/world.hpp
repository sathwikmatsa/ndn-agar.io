#pragma once
#include "agar.hpp"
#include "cell.hpp"
#include "context.hpp"
#include "projectile.hpp"
#include "virus.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class World {
  public:
    std::vector<Cell> pellets;
    std::vector<Virus> viruses;
    std::vector<Projectile> ejectiles;
    std::unique_ptr<Agar> agar;

    World();
    void update(Context &ctx);
    void render(Context &ctx, float fps);
    void handle_event(SDL_Event &e, Context &ctx);
};