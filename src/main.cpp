#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

#include "game_settings.hpp"
#include "context.hpp"
#include "cell_texture.hpp"
#include "agar.hpp"
#include "pellet.hpp"
#include "timer.hpp"
#include "grid.hpp"

float lerp(float start, float end, float t) {
    return start * (1 - t) + end * t;
}

int main(int argc, char* argv[]) {
    // init SDL, IMG
    Context ctx = Context();

    // Load Texture
    CellTexture txt("assets/circle.png", ctx);

    // setup timer
    Timer cap_timer;

    // Create a blob
    Agar agar("ABC");

    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distrx(0, PLAYGROUND_WIDTH - PELLET_RADIUS);
    std::uniform_int_distribution<> distry(0, PLAYGROUND_HEIGHT - PELLET_RADIUS);

    std::vector<Pellet> pellets;

    for(int n=0; n<300; ++n) {
        pellets.push_back(Pellet(distrx(eng), distry(eng)));
    }

    SDL_Event e;
    int mouse_x = SCREEN_WIDTH/2;
    int mouse_y = SCREEN_HEIGHT/2;

    float zoom = 1;

    bool running = true;
    while(running) {
        cap_timer.start();

        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                running = false;
            } else if(e.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouse_x, &mouse_y);
            }
        }

        agar.follow_mouse(mouse_x, mouse_y, ctx.camera);
        ctx.camera.set_center(agar.x, agar.y);

        SDL_SetRenderDrawColor(ctx.renderer, 242, 251, 255, 255);
        SDL_RenderClear(ctx.renderer);

        // draw grid
        Grid::draw(ctx);

        // render pellets and check if player eats any
        for(auto &pellet: pellets) {
            txt.render(pellet, ctx);
            if(agar.can_eat(pellet)) {
                agar.consume(pellet);
            }
        }

        float new_zoom = std::fmin(
            agar.get_size() / AGAR_RADIUS,
            2
        );

        zoom = lerp(zoom, new_zoom, 0.1);

        // shrink the world around
        ctx.camera.scale(
            zoom,
            ctx.renderer
        );

        // render player blob
        txt.render(agar, ctx);
        SDL_RenderPresent(ctx.renderer);

        // cap FPS
        int frame_ticks = cap_timer.get_ticks();
        if(frame_ticks < SCREEN_TICK_PER_FRAME) {
            SDL_Delay(SCREEN_TICK_PER_FRAME - frame_ticks);
        }
    }

    return 0;
}
