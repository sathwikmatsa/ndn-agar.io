#include <random>
#include "world.hpp"
#include "pellet.hpp"
#include "agar.hpp"
#include "context.hpp"
#include "game_settings.hpp"
#include "grid.hpp"

World::World() : agar("ABC") {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distrx(0, PLAYGROUND_WIDTH - PELLET_RADIUS);
    std::uniform_int_distribution<> distry(0, PLAYGROUND_HEIGHT - PELLET_RADIUS);

    for(int n=0; n<300; ++n) {
        pellets.push_back(Pellet(distrx(eng), distry(eng)));
    }
}

float lerp(float start, float end, float t) {
    return start * (1 - t) + end * t;
}

void World::update(Context& ctx) {
    // update player pos
    agar.follow_mouse(ctx.mouse_x, ctx.mouse_y, ctx.camera);
    ctx.camera.set_center(agar.x, agar.y);

    // check if player eats any pellets
    for(auto &pellet: pellets) {
        if(agar.can_eat(pellet)) {
            agar.consume(pellet);
        }
    }

    // adjust zoom
    float new_zoom = std::fmin(
        agar.get_size() / AGAR_RADIUS,
        2
    );

    ctx.zoom = lerp(ctx.zoom, new_zoom, 0.1);
}

void World::render(Context& ctx) {
    SDL_SetRenderDrawColor(ctx.renderer, 242, 251, 255, 255);
    // draw background
    SDL_RenderClear(ctx.renderer);

    // draw grid
    Grid::draw(ctx);

    CellTexture* texture = ctx.txt.get();

    // render pellets
    for(auto &pellet: pellets) {
        texture->render(pellet, ctx.camera, ctx.renderer);
    }

    // shrink the world around
    ctx.camera.scale(
        ctx.zoom,
        ctx.renderer
    );

    // render player
    texture->render(agar, ctx.camera, ctx.renderer);
    SDL_RenderPresent(ctx.renderer);
}
