#pragma once
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include "pellet.hpp"
#include "projectile.hpp"
#include "agar.hpp"
#include "context.hpp"

class World {
public:
    std::vector<std::unique_ptr<Pellet>> pellets;
    std::vector<std::unique_ptr<Projectile>> ejections;
    Agar agar;

    World();
    void update(Context& ctx);
    void render(Context& ctx);
    void handle_event(SDL_Event& e, Context& ctx);
};
