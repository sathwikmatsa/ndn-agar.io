#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>

#include "game_settings.hpp"
#include "context.hpp"
#include "blob_texture.hpp"
#include "blob.hpp"

int main(int argc, char* argv[]) {
    // init SDL, IMG
    Context ctx = Context();

    // Load Texture
    BlobTexture txt("assets/circle.png", ctx);

    // Create a blob
    Blob blob(240, 240, "ABC");

    bool running = true;
    SDL_Event e;

    while(running) {
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_RenderClear(ctx.renderer);
        // render blob
        txt.render(blob, ctx);
        SDL_RenderPresent(ctx.renderer);
    }

    return 0;
}
