#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <random>

#include "game_settings.hpp"
#include "context.hpp"
#include "blob_texture.hpp"
#include "blob.hpp"
#include "timer.hpp"

int main(int argc, char* argv[]) {
    // init SDL, IMG
    Context ctx = Context();

    // Load Texture
    BlobTexture txt("assets/circle.png", ctx);

    // setup timer
    Timer cap_timer;

    // Create a blob
    Blob blob(240, 240, "ABC");

    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distrx(0, PLAYGROUND_WIDTH - STATIC_BLOB_RADIUS);
    std::uniform_int_distribution<> distry(0, PLAYGROUND_HEIGHT - STATIC_BLOB_RADIUS);

    std::vector<Blob> npc_blobs;

    for(int n=0; n<300; ++n) {
        npc_blobs.push_back(Blob(distrx(eng), distry(eng)));
    }

    SDL_Event e;
    int mouse_x = 240;
    int mouse_y = 240;

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

        SDL_RenderClear(ctx.renderer);
        blob.follow_mouse(mouse_x, mouse_y, ctx);
        ctx.camera.set_center(blob.x, blob.y);
        // render npc blobs and check if player eats any
        for(auto &b: npc_blobs) {
            txt.render(b, ctx);
            if(blob.can_eat(b)) {
                blob.consume(b);
            }
        }
        // render player blob
        txt.render(blob, ctx);
        SDL_RenderPresent(ctx.renderer);


        // cap FPS
        int frame_ticks = cap_timer.get_ticks();
        if(frame_ticks < SCREEN_TICK_PER_FRAME) {
            SDL_Delay(SCREEN_TICK_PER_FRAME - frame_ticks);
        }
    }

    return 0;
}
