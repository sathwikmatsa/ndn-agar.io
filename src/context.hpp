#pragma once
#include <SDL2/SDL.h>

class Context {
public:
    // The window we'll be rendering to
    SDL_Window* window;
    // The window renderer
    SDL_Renderer* renderer;

    Context();
    ~Context();

private:
    // noob's attempt to make sure only one instance of Context is used.
    // Singleton design pattern utilizes static instance which means it's
    // destroyed after main (it's destructor is called after main returns)
    // and it somehow results in undefined behavior.
    static bool initialized;
};
