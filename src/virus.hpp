#pragma once
#include <cmath>

class Virus {
  public:
    float x, y;
    float radius;
    Virus(float px, float py, float r) : x(px), y(py), radius(r) {}
};
