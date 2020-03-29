#pragma once
#include "cell.hpp"

class Pellet: public Cell {
public:
    Pellet(int x, int y);
    float get_size();
    void succumb();
};
