#pragma once

enum class CellType {
  Player,
  Ejectile,
  Pellet,
};

struct CellSchema {
  CellType type;
  float x;
  float y;
  int r;
  int g;
  int b;
  float radius;
};
