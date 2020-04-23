#pragma once
#include <random>
#include <string>
#include <tuple>
#include <vector>

typedef uint8_t red;
typedef uint8_t green;
typedef uint8_t blue;
typedef uint32_t seq_id;

class GameState {
public:
  std::vector<std::tuple<std::string, red, green, blue, seq_id>> players;
  std::vector<std::tuple<int, int, red, green, blue>> pellets;
  std::vector<std::tuple<int, int>> viruses;

  GameState();
  std::tuple<int, int> relocate_pellet(int id);

private:
  std::random_device rd;
  std::mt19937 eng;
  std::uniform_int_distribution<> distrx;
  std::uniform_int_distribution<> distry;
  std::uniform_int_distribution<> distrc;
};
