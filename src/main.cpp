#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <random>
#include <cmath>

#include "context.hpp"
#include "timer.hpp"
#include "world.hpp"
#include "game_settings.hpp"

int main(int argc, char* argv[]) {
    // init SDL, IMG
    Context ctx = Context();

    // init world
    World world = World();

    // setup timer
    Timer cap_timer;

    SDL_Event e;

    bool running = true;
    while(running) {
        cap_timer.start();

        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                running = false;
            } else {
                world.handle_event(e, ctx);
            }
        }

        world.update(ctx);
        world.render(ctx);

        // cap FPS
        int frame_ticks = cap_timer.get_ticks();
        if(frame_ticks < SCREEN_TICK_PER_FRAME) {
            SDL_Delay(SCREEN_TICK_PER_FRAME - frame_ticks);
        }
    }

    return 0;
}
