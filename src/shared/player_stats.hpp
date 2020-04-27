#pragma once
#include <tuple>
#include <vector>

struct PlayerStats {
  std::vector<std::tuple<float, float, float>> cells;
  std::vector<std::tuple<float, float>> ejectiles;

  void clear() {
    cells.clear();
    ejectiles.clear();
  }
};
