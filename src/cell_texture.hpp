#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "cell.hpp"
#include "camera.hpp"

// Texture wrapper class
class CellTexture {
public:
    CellTexture(std::string path_to_file, SDL_Renderer* renderer);
    ~CellTexture();
    void render(Cell& cell, Camera& camera, SDL_Renderer* renderer);
private:
    SDL_Texture* texture;
};
