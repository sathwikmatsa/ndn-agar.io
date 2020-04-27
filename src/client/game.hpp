#pragma once
#include "./../shared/game_settings.hpp"
#include "argparse.hpp"
#include "bot.hpp"
#include "context.hpp"
#include "network_client.hpp"
#include "timer.hpp"
#include "world.hpp"
#include <string>
#include <yojimbo/yojimbo.h>

class Game {
public:
  static void run(Config config) {
    InitializeYojimbo();
    NetworkClient nc(config.server_address);
    // init world
    World world = World(config.player_name, std::move(config.bot));
    nc.join_room(config.player_name, world);

    // init SDL, IMG
    Context ctx = Context();

    SDL_Event e;

    // setup timers
    uint32_t previous, current, lag, elapsed;
    Timer cap_timer;
    Timer fps_timer;

    int counted_frames = 0;
    fps_timer.start();

    // http://gameprogrammingpatterns.com/game-loop.html#play-catch-up
    previous = SDL_GetTicks();
    lag = 0;
    while (world.running) {
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
          world.running = false;
          break;
        } else {
          world.handle_event(e, ctx);
        }
      }

      while (lag >= MS_PER_UPDATE) {
        world.update(ctx, nc);
        nc.update(dt, world);
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

    nc.close_connection();
    ShutdownYojimbo();
  }
};
