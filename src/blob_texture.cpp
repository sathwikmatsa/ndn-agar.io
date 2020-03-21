#include "blob_texture.hpp"
#include "blob.hpp"
#include "context.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <iostream>

BlobTexture::BlobTexture(std::string path_to_file, Context& ctx) {
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
            ctx.renderer,
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

BlobTexture::~BlobTexture() {
    if(texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
}

void BlobTexture::render(Blob& blob, Context& ctx) {
    int r = blob.get_radius();
    SDL_Rect renderQuad = {
        blob.x - r -ctx.camera.x_offset(),
        blob.y - r -ctx.camera.y_offset(),
        2 * r,
        2 * r
    };
    SDL_RenderCopy(ctx.renderer, texture, NULL, &renderQuad);
}
