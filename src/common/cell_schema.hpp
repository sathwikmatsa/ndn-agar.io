#pragma once

enum class CellType {
  Player,
  Ejectile,
  Pellet,
};

struct CellSchema {
  CellType type;
  int x;
  int y;
  int r;
  int g;
  int b;
  float radius;
};
