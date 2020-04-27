#include "game_state.hpp"
#include "./../shared/game_settings.hpp"
#include <random>

GameState::GameState() : eng(rd()) {
  distrx = std::uniform_int_distribution<>(AGAR_RADIUS,
                                           PLAYGROUND_WIDTH - AGAR_RADIUS);
  distry = std::uniform_int_distribution<>(AGAR_RADIUS,
                                           PLAYGROUND_HEIGHT - AGAR_RADIUS);
  distrc = std::uniform_int_distribution<>(0, 255);

  for (int i = 0; i < N_PELLETS; i++) {
    pellets.push_back(std::make_tuple(distrx(eng), distry(eng), distrc(eng),
                                      distrc(eng), distrc(eng)));
  }

  for (int i = 0; i < N_VIRUSES; i++) {
    viruses.push_back(std::make_tuple(distrx(eng), distry(eng)));
  }
}

std::tuple<int, int> GameState::relocate_pellet(int id) {
  int new_x = distrx(eng);
  int new_y = distry(eng);
  std::get<0>(pellets[id]) = new_x;
  std::get<1>(pellets[id]) = new_y;

  return std::make_tuple(new_x, new_y);
}

std::vector<std::tuple<std::string, red, green, blue>>
GameState::get_players() {
  std::vector<std::tuple<std::string, red, green, blue>> players_vec;
  for (auto &player_info : players) {
    auto [name, r, g, b, _] = player_info;
    players_vec.emplace_back(name, r, g, b);
  }
  return players_vec;
}
