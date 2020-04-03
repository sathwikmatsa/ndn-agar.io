#pragma once
#include <string>
#include <tuple>
#include "context.hpp"
#include "cell.hpp"
#include "projectile.hpp"
#include <vector>

class Agar {
public:
    float get_size();
    int r, g, b;
    std::vector<Cell> cells;

    Agar(std::string name, CellSettings cs);

    void follow_mouse(int mx, int my, Camera& camera);
    std::vector<Projectile> eject(int mx, int my, Camera& camera);
    void consume(Cell& other_cell);
    std::tuple<int, int> get_center();

    void render(Context& ctx);
private:
    std::string player_name;
};
