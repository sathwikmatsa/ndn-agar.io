#pragma once
#include "context.hpp"
#include "world.fwd.h"
#include <vector>

class Bot {
public:
  virtual void set_mouse_pos(World &world __attribute__((unused)),
                             Context &ctx __attribute__((unused))){};
  virtual ~Bot(){};
};

class Goblin : public Bot {
public:
  void set_mouse_pos(World &world, Context &ctx) override;

private:
  std::vector<Cell>::const_iterator
  closest_pellet(const std::vector<Cell> &pellets, float x, float y);
};
