#pragma once
#include "camera.hpp"
#include "cell.hpp"
#include <SDL2/SDL.h>
#include <string>

// Texture wrapper class
class CellTexture {
  public:
    CellTexture(std::string path_to_file, SDL_Renderer *renderer);
    ~CellTexture();
    void render(Cell &cell, Camera &camera, SDL_Renderer *renderer);

  private:
    SDL_Texture *texture;
};
