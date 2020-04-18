#include "grid.hpp"
#include "./../common/game_settings.hpp"
#include <cmath>

void Grid::draw(Context &ctx) {
  int dx = ctx.camera.x_offset();
  int dy = ctx.camera.y_offset();
  float zoom = ctx.camera.current_scale;
  int cam_h = ctx.camera.height();
  int cam_w = ctx.camera.width();

  SDL_SetRenderDrawColor(ctx.renderer, 205, 205, 205, 250);

  // vertical lines
  for (int i = 0; i <= cam_w; i++) {
    if (((i + dx) % int(std::round(GRID_TILE_SIZE / zoom))) == 0) {
      SDL_RenderDrawLine(ctx.renderer, i, 0, i, cam_h);
    }
  }
  // horizontal lines
  for (int i = 0; i <= cam_h; i++) {
    if (((i + dy) % int(std::round(GRID_TILE_SIZE / zoom))) == 0) {
      SDL_RenderDrawLine(ctx.renderer, 0, i, cam_w, i);
    }
  }
}
