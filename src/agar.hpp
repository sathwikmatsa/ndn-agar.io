#pragma once
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
    std::vector<Projectile> projectiles;

    Agar(std::string name, CellSettings cs);

    void follow_mouse(int mx, int my, Camera &camera);
    std::vector<Projectile> eject(int mx, int my, Camera &camera);
    void split(int mx, int my, Camera &camera);
    void consume(Cell &other_cell);

    void update(Context &ctx, std::vector<Cell> &pellets);
    void render(Context &ctx);

  private:
    Timer merge_timer;
    std::string player_name;
    void adjust_camera(Context &ctx);
    std::tuple<int, int> get_center();
};
