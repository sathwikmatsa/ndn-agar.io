#include "text_texture.hpp"
#include "game_settings.hpp"
#include <iostream>

TextTexture::TextTexture(std::string path) {
    font = TTF_OpenFont(path.c_str(), 38);
    if (font == nullptr) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError()
                  << '\n';
    }
}

TextTexture::~TextTexture() {
    for (auto &[text, texture] : textures) {
        SDL_DestroyTexture(texture);
    }

    TTF_CloseFont(font);
}

void TextTexture::load_from_rendered_text(std::string text,
                                          SDL_Renderer *renderer) {
    // Render text surface with outline
    // https://gamedev.stackexchange.com/a/135807
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    TTF_SetFontOutline(font, TEXT_OUTLINE_SIZE);
    SDL_Surface *bg_surface = TTF_RenderText_Blended(font, text.c_str(), black);
    TTF_SetFontOutline(font, 0);
    SDL_Surface *fg_surface = TTF_RenderText_Blended(font, text.c_str(), white);
    SDL_Rect rect = {TEXT_OUTLINE_SIZE, TEXT_OUTLINE_SIZE, fg_surface->w,
                     fg_surface->h};

    // blit text onto its outline
    SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND);
    SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect);
    SDL_FreeSurface(fg_surface);

    if (bg_surface == nullptr) {
        std::cout << "Unable to render text surface! SDL_ttf Error: "
                  << TTF_GetError() << '\n';
    } else {
        // Create texture from surface pixels
        SDL_Texture *texture =
            SDL_CreateTextureFromSurface(renderer, bg_surface);
        if (texture == nullptr) {
            std::cout
                << "Unable to create texture from rendered text! SDL Error: "
                << SDL_GetError() << '\n';
        }
        // save it to textures map
        textures.insert_or_assign(text, texture);

        // Get rid of old surface
        SDL_FreeSurface(bg_surface);
    }
}

void TextTexture::render(std::string text, int cx, int cy, int width,
                         Camera &camera, SDL_Renderer *renderer) {
    if (!textures.contains(text))
        load_from_rendered_text(text, renderer);

    float scale = camera.current_scale;
    int w, h;
    SDL_QueryTexture(textures[text], NULL, NULL, &w, &h);
    int rw = width;
    int rh = h * width / w;
    SDL_Rect quad = {int((cx - w / 2) * scale), int((cy - h / 2) * scale),
                     int(rw * scale), int(rh * scale)};
    // Render to screen
    SDL_RenderCopy(renderer, textures[text], NULL, &quad);
}

void TextTexture::render_celltext(std::string text, Cell &cell, Camera &camera,
                                  SDL_Renderer *renderer) {
    if (!textures.contains(text))
        load_from_rendered_text(text, renderer);

    int w, h;
    SDL_QueryTexture(textures[text], NULL, NULL, &w, &h);
    float rwidth = 0.9 * cell.radius * 2.0;
    float rheight = h * rwidth / w;
    SDL_Rect quad = {static_cast<int>(cell.x - rwidth / 2 - camera.x_offset()),
                     static_cast<int>(cell.y - rheight / 2 - camera.y_offset()),
                     (int)rwidth, (int)rheight};
    // Render to screen
    SDL_RenderCopy(renderer, textures[text], NULL, &quad);
}
