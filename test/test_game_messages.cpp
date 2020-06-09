#include "./../src/shared/game_messages.hpp"
#include "./../src/shared/serialization.hpp"
#include <iostream>
#include <cassert>

int main() {
  Stream stream = {true};
  // encode
  NewPlayerMessage enpm;
  enpm.player_index = 9;
  enpm.player_name = "Dhruva";
  enpm.r = 125;
  enpm.g = 120;
  enpm.b = 12;

  enpm.serialize(stream);

  // decode
  stream.is_writing = false;
  NewPlayerMessage dnpm;
  dnpm.serialize(stream);

  // compare
  assert(enpm.player_index == dnpm.player_index);
  assert(enpm.player_name.compare(dnpm.player_name) == 0);
  assert(enpm.r == dnpm.r);
  assert(enpm.g == dnpm.g);
  assert(enpm.b == dnpm.b);

  std::cout << "All tests passed!" << std::endl;
  return 0;
}
