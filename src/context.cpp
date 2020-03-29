#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <memory>
#include "context.hpp"
#include "game_settings.hpp"

bool Context::initialized = false;

Context::Context() {
    if(!initialized) {
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "SDL could not initialize! SDL Error: "
                << SDL_GetError() << '\n';
        } else {
            if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
                std::cout << "Warning: Linear texture filtering not enabled!" << '\n';
            }

            window = SDL_CreateWindow(
                "Agar.io",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                SDL_WINDOW_SHOWN
            );

            if(window == nullptr) {
                std::cout << "Window could not be created! SDL Error: "
                    << SDL_GetError() << '\n';

            } else {
                renderer = SDL_CreateRenderer(
                    window,
                    -1,
                    SDL_RENDERER_ACCELERATED
                );
                if(renderer == nullptr) {
                    std::cout << "Renderer could not be created! SDL Error: "
                        << SDL_GetError() << '\n';
                } else {
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);

                    int imgFlags = IMG_INIT_PNG;
                    if(!(IMG_Init(imgFlags) & imgFlags)) {
                        std::cout << "SDL_image could not initialize! SDL_image Error: "
                            << IMG_GetError() << '\n';
                    }

                    txt =  std::make_unique<CellTexture>("assets/circle.png", renderer);
                    mouse_x = SCREEN_WIDTH/2;
                    mouse_y = SCREEN_HEIGHT/2;
                    zoom = 1;
                }
            }
        }
        initialized = true;
    } else {
        std::cout << "Can't initialize Context more than once!\n";
    }
}

Context::~Context() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}
