#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <yojimbo/yojimbo.h>

#include "context.hpp"
#include "./../common/game_settings.hpp"
#include "network_client.hpp"
#include "timer.hpp"
#include "world.hpp"

int main(int argc, char *argv[]) {
    std::string player_name;
    if(argc == 2) {
        player_name = std::string(argv[1]);
    } else {
        std::cout << "enter player name [max 7 characters]: ";
        std::cin >> player_name;
    }

    player_name = player_name.substr(0, 7);

    InitializeYojimbo();
    yojimbo::Address server_address(127, 0, 0, 1, 9999);
    NetworkClient nc(server_address);
    nc.run();

    // init SDL, IMG
    Context ctx = Context();

    // init world
    World world = World();

    SDL_Event e;

    // setup timers
    uint32_t previous, current, lag, elapsed;
    Timer cap_timer;
    Timer fps_timer;

    int counted_frames = 0;
    fps_timer.start();

    // http://gameprogrammingpatterns.com/game-loop.html#play-catch-up
    bool running = true;
    previous = SDL_GetTicks();
    lag = 0;
    while (running) {
        // calculate and correct fps
        float avg_fps = counted_frames / (fps_timer.get_ticks() / 1000.f);
        if (avg_fps > 2000000)
            avg_fps = 0;

        cap_timer.start();

        current = SDL_GetTicks();
        elapsed = current - previous;
        previous = current;
        lag += elapsed;

        // process user input
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else {
                world.handle_event(e, ctx);
            }
        }

        while (lag >= MS_PER_UPDATE) {
            world.update(ctx);
            lag -= MS_PER_UPDATE;
        }

        world.render(ctx, avg_fps);

        // cap FPS
        int frame_ticks = cap_timer.get_ticks();
        if (frame_ticks < SCREEN_TICK_PER_FRAME) {
            SDL_Delay(SCREEN_TICK_PER_FRAME - frame_ticks);
        }

        ++counted_frames;
    }

    ShutdownYojimbo();
    return 0;
}
