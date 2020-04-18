#include "virus_texture.hpp"
#include "camera.hpp"
#include "virus.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>

VirusTexture::VirusTexture(std::string path_to_file, SDL_Renderer *renderer) {
  texture = nullptr;
  SDL_Surface *loadedSurface = IMG_Load(path_to_file.c_str());

  if (loadedSurface == nullptr) {
    std::cout << "Unable to load image: " << path_to_file
              << " SDL_image Error: " << IMG_GetError() << '\n';
  } else {
    // Color key with black color
    SDL_SetColorKey(loadedSurface, SDL_TRUE,
                    SDL_MapRGB(loadedSurface->format, 0, 0, 0));

    // create texture from surface pixels
    texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

    if (texture == nullptr) {
      std::cout << "Unable to create texture from " << path_to_file
                << "SDL Error: " << SDL_GetError() << '\n';
    }

    SDL_FreeSurface(loadedSurface);
  }
}

VirusTexture::~VirusTexture() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
  }
}

void VirusTexture::render(Virus &virus, Camera &camera,
                          SDL_Renderer *renderer) {
  float r = virus.radius;
  SDL_Rect renderQuad = {static_cast<int>(virus.x - r - camera.x_offset()),
                         static_cast<int>(virus.y - r - camera.y_offset()),
                         static_cast<int>(2 * r), static_cast<int>(2 * r)};
  SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
}
