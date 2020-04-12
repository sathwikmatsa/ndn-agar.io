#pragma once
#include "camera.hpp"
#include "virus.hpp"
#include <SDL2/SDL.h>
#include <string>

class VirusTexture {
  public:
    VirusTexture(std::string path_to_file, SDL_Renderer *renderer);
    ~VirusTexture();
    void render(Virus &virus, Camera &camera, SDL_Renderer *renderer);

  private:
    SDL_Texture *texture;
};
