#pragma once
#include "./../shared/player_stats.hpp"
#include "agar.hpp"
#include "bot.hpp"
#include "cell.hpp"
#include "context.hpp"
#include "network_client.fwd.h"
#include "projectile.hpp"
#include "timer.hpp"
#include "virus.hpp"
#include <SDL2/SDL.h>
#include <memory>
#include <tuple>
#include <vector>

class World {
public:
  std::vector<Cell> pellets;
  std::vector<Virus> viruses;
  std::unique_ptr<Agar> agar;

  int my_index;
  std::vector<std::tuple<bool, std::string, uint8_t, uint8_t, uint8_t>>
      players_info;
  std::vector<PlayerStats> players_stats;

  bool running;

  World(std::string player_name, std::unique_ptr<Bot> bot);
  void update(Context &ctx, NetworkClient &nc);
  void render(Context &ctx, float fps);
  void handle_event(SDL_Event &e, Context &ctx);

  void add_player(std::tuple<std::string, uint8_t, uint8_t, uint8_t> player);
  void create_pellet(std::tuple<int, int, uint8_t, uint8_t, uint8_t> pellet);
  void create_virus(std::tuple<int, int> virus);
  void relocate_pellet(int id, int x, int y);

private:
  std::unique_ptr<Bot> bot;
  Timer interaction_timer;
  void render_online_players(Context &);
  void render_cells(Context &, std::vector<std::tuple<float, float, float>> &,
                    std::string, uint8_t, uint8_t, uint8_t);
  void render_ejectiles(Context &, std::vector<std::tuple<float, float>> &,
                        uint8_t, uint8_t, uint8_t);
};
