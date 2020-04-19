#pragma once
#include "camera.hpp"
#include "cell.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <string>

class TextTexture {
public:
  TextTexture(std::string path);
  ~TextTexture();

  // Creates image from font string
  void load_from_rendered_text(std::string text, SDL_Renderer *renderer);

  // Renders text texture at the given coordinates as centers
  void render(std::string text, int cx, int cy, int width, Camera &camera,
              SDL_Renderer *renderer);

  // Renders text on cell
  void render_celltext(std::string text, Cell &cell, Camera &camera,
                       SDL_Renderer *renderer);

  void render_celltext(std::string text, CellSchema &schema, Camera &camera,
                       SDL_Renderer *renderer);

private:
  std::map<std::string, SDL_Texture *> textures;
  TTF_Font *font;
};
