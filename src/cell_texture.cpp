#include "cell_texture.hpp"
#include "cell.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>
#include "camera.hpp"

CellTexture::CellTexture(std::string path_to_file, SDL_Renderer* renderer) {
    texture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path_to_file.c_str());

    if(loadedSurface == nullptr) {
        std::cout << "Unable to load image: "
            << path_to_file << " SDL_image Error: "
            << IMG_GetError() << '\n';
    } else {
        // Color key with black color
        SDL_SetColorKey(
            loadedSurface,
            SDL_TRUE,
            SDL_MapRGB(loadedSurface->format, 0, 0, 0)
        );

        // create texture from surface pixels
        texture = SDL_CreateTextureFromSurface(
            renderer,
            loadedSurface
        );

        if(texture == nullptr) {
            std::cout << "Unable to create texture from "
                << path_to_file << "SDL Error: "
                << SDL_GetError() << '\n';
        }

        SDL_FreeSurface(loadedSurface);
    }
}

CellTexture::~CellTexture() {
    if(texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
}

void CellTexture::render(Cell& cell, Camera& camera, SDL_Renderer* renderer) {
    float r = cell.get_size();
    SDL_Rect renderQuad = {
        static_cast<int>(cell.x - r -camera.x_offset()),
        static_cast<int>(cell.y - r -camera.y_offset()),
        static_cast<int>(2 * r),
        static_cast<int>(2 * r)
    };
    SDL_SetTextureColorMod( texture, cell.r, cell.g, cell.b );
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
}
