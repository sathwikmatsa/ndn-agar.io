#include "bot.hpp"
#include "world.hpp"
#include <algorithm>
#include <cmath>

void Goblin::set_mouse_pos(World &world, Context &ctx) {
  auto x = (*world.agar).cells[0].x;
  auto y = (*world.agar).cells[0].y;
  Cell &pellet = closest_pellet(world.pellets, x, y);
  ctx.mouse_x = (pellet.x - ctx.camera.x_offset()) / ctx.zoom;
  ctx.mouse_y = (pellet.y - ctx.camera.y_offset()) / ctx.zoom;
}

Cell &Goblin::closest_pellet(std::vector<Cell> &pellets, float x, float y) {
  auto pellet_i = std::min_element(
      pellets.begin(), pellets.end(), [=](const Cell &a, const Cell &b) {
        return std::pow(a.x - x, 2) + std::pow(a.y - y, 2) <
               std::pow(b.x - x, 2) + std::pow(b.y - y, 2);
      });

  return *pellet_i;
}
