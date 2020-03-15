#pragma once
#include <SDL2/SDL.h>

class Context {
public:
    // The window we'll be rendering to
    SDL_Window* window;
    // The window renderer
    SDL_Renderer* renderer;

    ~Context();
};
