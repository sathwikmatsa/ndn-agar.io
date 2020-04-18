#pragma once
#include "agar.hpp"
#include "cell.hpp"
#include "context.hpp"
#include "network_client.fwd.h"
#include "projectile.hpp"
#include "virus.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <vector>

class World {
public:
  std::vector<Cell> pellets;
  std::vector<Virus> viruses;
  std::vector<Projectile> ejectiles;
  std::unique_ptr<Agar> agar;
  bool running;

  World(std::string player_name);
  void update(Context &ctx, NetworkClient &nc);
  void render(Context &ctx, float fps);
  void handle_event(SDL_Event &e, Context &ctx);

  void create_pellet(std::tuple<int, int, uint8_t, uint8_t, uint8_t> pellet);
  void create_virus(std::tuple<int, int> virus);
  void relocate_pellet(int id, int x, int y);
};
