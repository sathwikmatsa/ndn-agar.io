#pragma once
#include <vector>
#include "pellet.hpp"
#include "agar.hpp"
#include "context.hpp"

class World {
public:
    std::vector<Pellet> pellets;
    std::vector<Pellet> ejections;
    Agar agar;

    World();
    void update(Context& ctx);
    void render(Context& ctx);
};
