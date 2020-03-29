#pragma once
#include <string>
#include "context.hpp"
#include "cell.hpp"

class Agar: public Cell {
public:
    void follow_mouse(int mx, int my, Camera& camera);
    bool can_eat(Cell& other_cell);
    void consume(Cell& other_cell);
    float get_size();
    void succumb();
    Agar(std::string name);
private:
    std::string player_name;
};
