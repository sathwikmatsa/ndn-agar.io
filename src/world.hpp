#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "projectile.hpp"
#include "agar.hpp"
#include "context.hpp"

class World {
public:
    std::vector<Cell> pellets;
    std::vector<Projectile> ejectiles;
    std::unique_ptr<Agar> agar;

    World();
    void update(Context& ctx);
    void render(Context& ctx);
    void handle_event(SDL_Event& e, Context& ctx);
};
