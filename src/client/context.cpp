#include "context.hpp"
#include "game_settings.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <memory>

bool Context::initialized = false;

Context::Context() {
    if (!initialized) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cout << "SDL could not initialize! SDL Error: "
                      << SDL_GetError() << '\n';
        } else {
            if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
                std::cout << "Warning: Linear texture filtering not enabled!"
                          << '\n';
            }

            window = SDL_CreateWindow("Agar.io", SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                      SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

            if (window == nullptr) {
                std::cout << "Window could not be created! SDL Error: "
                          << SDL_GetError() << '\n';

            } else {
                renderer =
                    SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
                if (renderer == nullptr) {
                    std::cout << "Renderer could not be created! SDL Error: "
                              << SDL_GetError() << '\n';
                } else {
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF,
                                           SDL_ALPHA_OPAQUE);

                    int imgFlags = IMG_INIT_PNG;
                    if (!(IMG_Init(imgFlags) & imgFlags)) {
                        std::cout << "SDL_image could not initialize! "
                                     "SDL_image Error: "
                                  << IMG_GetError() << '\n';
                    }

                    if (TTF_Init() < 0) {
                        std::cout
                            << "SDL_ttf could not initialize! SDL_ttf Error: "
                            << TTF_GetError() << '\n';
                    }

                    txt = new CellTexture("assets/circle.png", renderer);
                    vtxt = new VirusTexture("assets/virus.png", renderer);
                    ttxt = new TextTexture("assets/font.ttf");
                    mouse_x = SCREEN_WIDTH / 2;
                    mouse_y = SCREEN_HEIGHT / 2;
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
    delete txt;
    delete vtxt;
    delete ttxt;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
