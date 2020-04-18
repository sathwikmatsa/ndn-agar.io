#pragma once
#include "camera.hpp"
#include "cell_texture.hpp"
#include "text_texture.hpp"
#include "virus_texture.hpp"
#include <SDL2/SDL.h>
#include <memory>

class Context {
public:
  // The window we'll be rendering to
  SDL_Window *window;
  // The window renderer
  SDL_Renderer *renderer;

  Camera camera;

  CellTexture *txt;
  VirusTexture *vtxt;
  TextTexture *ttxt;

  int mouse_x, mouse_y;
  float zoom;

  Context();
  ~Context();

private:
  // noob's attempt to make sure only one instance of Context is used.
  // Singleton design pattern utilizes static instance which means it's
  // destroyed after main (it's destructor is called after main returns)
  // and it somehow results in undefined behavior.
  static bool initialized;
};
