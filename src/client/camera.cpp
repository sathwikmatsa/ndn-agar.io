#include "camera.hpp"
#include "./../common/game_settings.hpp"
#include <SDL2/SDL.h>
#include <cmath>

Camera::Camera() {
  current_scale = 1;
  camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
}

bool Camera::at_boundary() { return camera_at_boundary; }

int Camera::width() { return camera.w; }

int Camera::height() { return camera.h; }

int Camera::x_offset() { return camera.x; }

int Camera::y_offset() { return camera.y; }

void Camera::scale(float scale_factor, SDL_Renderer *renderer) {
  int new_width = SCREEN_WIDTH * scale_factor;
  int new_height = SCREEN_HEIGHT * scale_factor;

  SDL_RenderSetLogicalSize(renderer, new_width, new_height);

  camera.w = new_width;
  camera.h = new_height;
  current_scale = scale_factor;
}

void Camera::set_center(int cx, int cy) {
  camera.x = std::lerp(camera.x, cx - camera.w / 2, 0.05f);
  camera.y = std::lerp(camera.y, cy - camera.h / 2, 0.05f);
  camera_at_boundary = false;

  if (camera.x < 0) {
    camera.x = 0;
    camera_at_boundary = true;
  }
  if (camera.y < 0) {
    camera.y = 0;
    camera_at_boundary = true;
  }
  if (camera.x > (PLAYGROUND_WIDTH - camera.w)) {
    camera.x = PLAYGROUND_WIDTH - camera.w;
    camera_at_boundary = true;
  }
  if (camera.y > (PLAYGROUND_HEIGHT - camera.h)) {
    camera.y = PLAYGROUND_HEIGHT - camera.h;
    camera_at_boundary = true;
  }
}
