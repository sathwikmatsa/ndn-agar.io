#include <SDL2/SDL.h>
#include "context.hpp"

Context::~Context() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
