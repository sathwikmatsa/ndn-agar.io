#pragma once
#include <random>
#include <string>
#include <tuple>
#include <vector>

class GameState {
public:
  std::vector<std::string> players;
  std::vector<std::tuple<int, int, uint8_t, uint8_t, uint8_t>> pellets;
  std::vector<std::tuple<int, int>> viruses;

  GameState();
  std::tuple<int, int> relocate_pellet(int id);
  void add_player(std::string name);

private:
  std::random_device rd;
  std::mt19937 eng;
  std::uniform_int_distribution<> distrx;
  std::uniform_int_distribution<> distry;
  std::uniform_int_distribution<> distrc;
};
