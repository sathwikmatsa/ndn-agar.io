#pragma once
#include "cell.hpp"
#include <cmath>

class Virus {
  public:
    float x, y;
    float radius;
    Virus(float px, float py, float r) : x(px), y(py), radius(r) {}
    bool can_disintegrate(Cell &other_cell) {
        if (other_cell.radius > radius) {
            float dist = std::sqrt(std::pow(other_cell.x - x, 2) +
                                   std::pow(other_cell.y - y, 2));
            return dist < other_cell.radius;
        }
        return false;
    }
};
