#include "world.hpp"
#include "agar.hpp"
#include "cell.hpp"
#include "context.hpp"
#include "game_settings.hpp"
#include "grid.hpp"
#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

World::World() {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distrx(0, PLAYGROUND_WIDTH - AGAR_RADIUS);
    std::uniform_int_distribution<> distry(0, PLAYGROUND_HEIGHT - AGAR_RADIUS);
    std::uniform_int_distribution<> distrc(0, 255);

    agar = std::unique_ptr<Agar>(
        new Agar("ABC", {CellType::Player, distrx(eng), distry(eng),
                         distrc(eng), distrc(eng), distrc(eng), AGAR_RADIUS}));

    for (int n = 0; n < 1000; ++n) {
        CellSettings cs = {CellType::Pellet, distrx(eng), distry(eng),
                           distrc(eng),      distrc(eng), distrc(eng),
                           PELLET_RADIUS};
        pellets.emplace_back(cs);
    }
}

void World::update(Context &ctx) {
    // update player
    (*agar).update(ctx, pellets);

    // update ejectiles
    for (auto &projectile : ejectiles) {
        projectile.update_pos();
    }

    // move projectile to pellets vec after coming to rest
    std::erase_if(ejectiles, [this](Projectile &projectile) {
        bool pred = projectile.at_rest();
        if (pred) {
            pellets.push_back(std::move(projectile.cell));
        }
        return pred;
    });

    // remove pellets with zero mass
    std::erase_if(pellets, [](Cell &cell) { return cell.radius == 0; });
}

void World::render(Context &ctx) {
    SDL_SetRenderDrawColor(ctx.renderer, 242, 251, 255, 255);
    // draw background
    SDL_RenderClear(ctx.renderer);

    // draw grid
    Grid::draw(ctx);

    CellTexture *texture = ctx.txt;

    // render pellets
    for (auto &pellet : pellets) {
        texture->render(pellet, ctx.camera, ctx.renderer);
    }

    // render ejectiles
    for (auto &projectile : ejectiles) {
        texture->render(projectile.cell, ctx.camera, ctx.renderer);
    }

    // shrink the world around
    ctx.camera.scale(ctx.zoom, ctx.renderer);

    // render player
    (*agar).render(ctx);
    SDL_RenderPresent(ctx.renderer);
}

void World::handle_event(SDL_Event &e, Context &ctx) {
    if (e.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&ctx.mouse_x, &ctx.mouse_y);
    } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w: {
            auto ejections =
                (*agar).eject(ctx.mouse_x, ctx.mouse_y, ctx.camera);
            for (auto &ejection : ejections) {
                ejectiles.push_back(std::move(ejection));
            }
            break;
        }
        case SDLK_SPACE: {
            (*agar).split(ctx.mouse_x, ctx.mouse_y, ctx.camera);
            break;
        }
        }
    }
}
