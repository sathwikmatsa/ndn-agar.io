#include <random>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <SDL2/SDL.h>
#include "world.hpp"
#include "cell.hpp"
#include "agar.hpp"
#include "context.hpp"
#include "game_settings.hpp"
#include "grid.hpp"

World::World() {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distrx(0, PLAYGROUND_WIDTH - AGAR_RADIUS);
    std::uniform_int_distribution<> distry(0, PLAYGROUND_HEIGHT - AGAR_RADIUS);
    std::uniform_int_distribution<> distrc(0, 255);

    agar = std::unique_ptr<Agar>(new Agar(
        "ABC",
        {
            CellType::Pellet,
            distrx(eng),
            distry(eng),
            distrc(eng),
            distrc(eng),
            distrc(eng),
            AGAR_RADIUS
        }
    ));

    for(int n=0; n<1000; ++n) {
        CellSettings cs = {
            CellType::Pellet,
            distrx(eng),
            distry(eng),
            distrc(eng),
            distrc(eng),
            distrc(eng),
            PELLET_RADIUS
        };
        pellets.emplace_back(cs);
    }
}

void World::update(Context& ctx) {
    // update player pos
    (*agar).follow_mouse(ctx.mouse_x, ctx.mouse_y, ctx.camera);
    auto [agar_cx, agar_cy] = (*agar).get_center();
    ctx.camera.set_center(agar_cx, agar_cy);

    // update projectiles
    for(auto &projectile: (*agar).projectiles) {
        projectile.update_pos();
    }
    for(auto &projectile: ejectiles) {
        projectile.update_pos();
    }

    // move projectile to pellets vec after coming to rest
    std::erase_if(ejectiles, [this](Projectile& projectile)
        {
            bool pred = projectile.at_rest();
            if(pred) {
                pellets.push_back(std::move(projectile.cell));
            }
            return pred;
        }
    );

    // Agar: move projectile to cells vec after coming to rest
    std::erase_if((*agar).projectiles, [this](Projectile& projectile)
        {
            bool pred = projectile.at_rest();
            if(pred) {
                (*agar).cells.push_back(std::move(projectile.cell));
            }
            return pred;
        }
    );

    // check if player eats any pellets
    for(auto &pellet: pellets) {
        for(auto &cell: (*agar).cells) {
            if(cell.can_eat(pellet)) {
                cell.consume(pellet);
            }
        }
        for(auto &projectile: (*agar).projectiles) {
            if(projectile.cell.can_eat(pellet)) {
                projectile.cell.consume(pellet);
            }
        }
    }

    // adjust zoom
    float new_zoom = std::fmin(
        (*agar).get_size() / AGAR_RADIUS,
        1.5
    );

    ctx.zoom = std::lerp(ctx.zoom, new_zoom, 0.05f);
}

void World::render(Context& ctx) {
    SDL_SetRenderDrawColor(ctx.renderer, 242, 251, 255, 255);
    // draw background
    SDL_RenderClear(ctx.renderer);

    // draw grid
    Grid::draw(ctx);

    CellTexture* texture = ctx.txt;

    // render pellets
    for(auto &pellet: pellets) {
        texture->render(pellet, ctx.camera, ctx.renderer);
    }

    // render ejectiles
    for(auto &projectile: ejectiles) {
        texture->render(projectile.cell, ctx.camera, ctx.renderer);
    }

    // shrink the world around
    ctx.camera.scale(
        ctx.zoom,
        ctx.renderer
    );

    // render player
    (*agar).render(ctx);
    SDL_RenderPresent(ctx.renderer);
}

void World::handle_event(SDL_Event& e, Context& ctx) {
    if(e.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&ctx.mouse_x, &ctx.mouse_y);
    } else if (e.type == SDL_KEYDOWN) {
        switch(e.key.keysym.sym) {
            case SDLK_w:
            {
                auto ejections = (*agar).eject(ctx.mouse_x, ctx.mouse_y, ctx.camera);
                for(auto &ejection: ejections) {
                    ejectiles.push_back(std::move(ejection));
                }
                break;
            }
            case SDLK_SPACE:
            {
                (*agar).split(ctx.mouse_x, ctx.mouse_y, ctx.camera);
                break;
            }
        }
    }
}
