#pragma once
#include <string>
#include <tuple>
#include "context.hpp"
#include "cell.hpp"
#include "projectile.hpp"
#include <vector>

class Agar {
public:
    void follow_mouse(int mx, int my, Camera& camera);
    std::vector<std::unique_ptr<Projectile>>
    eject(int mx, int my, Camera& camera);
    void consume(Cell& other_cell);
    void render(Context& ctx);
    std::tuple<int, int> get_center();
    Agar(std::string name, CellSettings cs);
    std::vector<Cell> cells;
    float get_size();
    int r, g, b;
private:
    std::string player_name;
};
