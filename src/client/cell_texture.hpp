#pragma once
#include "camera.hpp"
#include "cell.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <tuple>

// Texture wrapper class
class CellTexture {
public:
  CellTexture(std::string path_to_file, SDL_Renderer *renderer);
  ~CellTexture();
  void render(Cell &cell, Camera &camera, SDL_Renderer *renderer);
  void render(std::tuple<float, float, float> &, uint8_t, uint8_t, uint8_t,
              Camera &, SDL_Renderer *);

private:
  SDL_Texture *texture;
};
