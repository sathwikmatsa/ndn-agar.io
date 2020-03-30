#include <random>
#include <vector>
#include <memory>
#include <cmath>
#include <SDL2/SDL.h>
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
        pellets.push_back(std::make_unique<Pellet>(distrx(eng), distry(eng)));
    }
}

void World::update(Context& ctx) {
    // update player pos
    agar.follow_mouse(ctx.mouse_x, ctx.mouse_y, ctx.camera);
    ctx.camera.set_center(agar.x, agar.y);

    // update projectiles
    for(auto &projectile: ejections) {
        (*projectile).update_pos();
    }

    // move projectile to pellets vec after coming to rest
    std::erase_if(ejections, [this](
        std::unique_ptr<Projectile>& projectile) {
            bool pred = (*projectile).at_rest();
            if(pred) {
                Cell* cell = (*projectile).cell.release();
                Pellet* pellet = reinterpret_cast<Pellet*>(cell);
                std::unique_ptr<Pellet> pellet_ptr(pellet);
                pellets.push_back(std::move(pellet_ptr));
            }
            return pred;
        }
    );

    // check if player eats any pellets
    for(auto &pellet: pellets) {
        if(agar.can_eat(*pellet)) {
            agar.consume(*pellet);
        }
    }

    // adjust zoom
    float new_zoom = std::fmin(
        agar.get_size() / AGAR_RADIUS,
        1.5
    );

    ctx.zoom = std::lerp(ctx.zoom, new_zoom, 0.1f);
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
        texture->render(*pellet, ctx.camera, ctx.renderer);
    }

    // render ejections
    for(auto &projectile: ejections) {
        texture->render(*(*projectile).cell, ctx.camera, ctx.renderer);
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

void World::handle_event(SDL_Event& e, Context& ctx) {
    if(e.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&ctx.mouse_x, &ctx.mouse_y);
    } else if (e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
            case SDLK_w:
            {
                auto try_eject = agar.eject(ctx.mouse_x, ctx.mouse_y, ctx.camera);
                if(try_eject.has_value()) {
                    ejections.push_back(std::move(try_eject.value()));
                }
                break;
            }
        }
    }
}
