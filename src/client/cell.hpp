#pragma once
#include <cmath>

enum class CellType {
  Player,
  Ejectile,
  Pellet,
};

struct CellSettings {
  CellType type;
  int x;
  int y;
  int r;
  int g;
  int b;
  float radius;
};

class Cell {
public:
  float x, y;
  int r, g, b;
  float radius;
  bool active;
  CellType type;
  virtual ~Cell() {}
  float get_mass() {
    switch (type) {
    case CellType::Pellet:
      return radius / 2;
    default:
      return radius;
    }
  }
  virtual void succumb() { radius = 0; }
  bool can_eat(Cell &other_cell) {
    float r2 = other_cell.radius;
    float distance_centers = std::sqrt(std::pow(x - other_cell.x, 2) +
                                       std::pow(y - other_cell.y, 2));
    if (distance_centers + r2 < radius && active)
      return true;
    else
      return false;
  }
  void consume(Cell &other_cell) {
    float r2 = other_cell.get_mass();
    radius = std::sqrt(radius * radius + r2 * r2);
    other_cell.succumb();
  }
  float dist(Cell &other_cell) {
    return std::sqrt(std::pow(x - other_cell.x, 2) +
                     std::pow(y - other_cell.y, 2));
  }
  bool overlaps(Cell &other_cell) {
    return dist(other_cell) < radius + other_cell.radius;
  }
  void touch_boundary(Cell &other_cell) {
    float vec_x = x - other_cell.x;
    float vec_y = y - other_cell.y;
    float vec_mag = std::sqrt(std::pow(vec_x, 2) + std::pow(vec_y, 2));
    float back_off_dist = radius + other_cell.radius - dist(other_cell);
    x += (vec_x / vec_mag) * back_off_dist;
    y += (vec_y / vec_mag) * back_off_dist;
  }
  Cell(CellSettings s)
      : x(s.x), y(s.y), r(s.r), g(s.g), b(s.b), radius(s.radius), active(true),
        type(s.type) {}
};
