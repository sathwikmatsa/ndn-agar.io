#pragma once
#include "./../shared/player_stats.hpp"
#include "cell.hpp"
#include "context.hpp"
#include "projectile.hpp"
#include "timer.hpp"
#include <string>
#include <tuple>
#include <vector>

class Agar {
public:
  float get_size();
  int r, g, b;
  std::vector<Cell> cells;
  std::vector<Cell> ejectiles;
  std::vector<Projectile> projectiles;

  Agar(std::string name, CellSchema cs);

  void follow_mouse(int mx, int my, Camera &camera);
  void eject(int mx, int my, Camera &camera);
  void split(int mx, int my, Camera &camera);
  void consume(Cell &other_cell);
  void disintegrate_cell(Cell &cell);

  void update(Context &ctx, std::vector<Cell> &pellets,
              const std::vector<Virus> &viruses);
  void render(Context &ctx);

  PlayerStats get_player_stats();

private:
  Timer merge_timer;
  std::string player_name;
  void adjust_camera(Context &ctx);
  std::tuple<int, int> get_center();
};
