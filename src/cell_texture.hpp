#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "cell.hpp"
#include "context.hpp"

// Texture wrapper class
class CellTexture {
public:
    CellTexture(std::string path_to_file, Context& ctx);
    ~CellTexture();
    void render(Cell& cell, Context& ctx);
private:
    SDL_Texture* texture;
};
