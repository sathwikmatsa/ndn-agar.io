#pragma once
#include "cell.hpp"

class Pellet: public Cell {
public:
    Pellet(int x, int y);
    Pellet(int x, int y, int r, int g, int b);
    float get_size();
    void succumb();
};
