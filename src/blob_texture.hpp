#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "blob.hpp"
#include "context.hpp"

// Texture wrapper class
class BlobTexture {
public:
    BlobTexture(std::string path_to_file, Context& ctx);
    ~BlobTexture();
    void render(Blob& blob, Context& ctx);
private:
    SDL_Texture* texture;
};
