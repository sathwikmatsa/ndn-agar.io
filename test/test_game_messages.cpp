#include "./../src/shared/game_messages.hpp"
#include "./../src/shared/serialization.hpp"
#include <iostream>
#include <cassert>

int main() {

  std::cout << "Running test: NewPlayerMessage\t";
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
  std::cout << "OK" << std::endl;

  /* =================================================== */
  
  std::cout << "Running test: GameInfoMessage\t";
  stream={true};
  // encode
  GameInfoMessage egim;
  int r=125,g=120,b=12;
  int x=10,y=20;
  egim.player_index = 9;
  egim.players = std::vector<std::tuple<std::string, uint8_t, uint8_t, uint8_t>>(2,std::make_tuple("Dhruva",r,g,b));
  egim.pellets = std::vector<std::tuple<int, int, uint8_t, uint8_t, uint8_t>>(2,std::make_tuple(x,y,r,g,b));
  egim.viruses = std::vector<std::tuple<int, int>>(2,std::make_tuple(x,y));

  egim.serialize(stream);
  
  //decode
  stream.is_writing = false;
  GameInfoMessage dgim;
  dgim.serialize(stream);
  assert(egim.player_index == dgim.player_index);
  assert(egim.players == dgim.players);
  assert(egim.pellets == dgim.pellets);
  assert(egim.viruses == dgim.viruses);
  std::cout << "OK" << std::endl;

  /* =================================================== */
  
  std::cout << "Running test: AtePelletMessage\t";
  stream={true};
  // encode
  AtePelletMessage eapm;
  eapm.pellet_id = 9;
  eapm.serialize(stream);
  //decode
  stream.is_writing = false;
  AtePelletMessage dapm;
  dapm.serialize(stream);
  assert(eapm.pellet_id == dapm.pellet_id);
  std::cout << "OK" << std::endl;

  /* =================================================== */
  
  std::cout << "Running test: PelletRelocMessage\t";
  stream={true};
  // encode
  PelletRelocMessage eprm;
  eprm.pellet_id = 9;
  eprm.pos_x=10;
  eprm.pos_y=10;
  eprm.serialize(stream);
  //decode
  stream.is_writing = false;
  PelletRelocMessage dprm;
  dprm.serialize(stream);
  assert(eprm.pellet_id == dprm.pellet_id);
  assert(eprm.pos_x == dprm.pos_x);
  assert(eprm.pos_y == dprm.pos_y);
  std::cout << "OK" << std::endl;

  /* =================================================== */
  
  std::cout << "Running test: GameOverMessage\t";
  stream={true};
  // encode
  GameOverMessage egom;
  egom.rank = 9;
  eapm.serialize(stream);
  //decode
  stream.is_writing = false;
  GameOverMessage dgom;
  dgom.serialize(stream);
  assert(egom.rank == dgom.rank);
  std::cout << "OK" << std::endl;

  /* =================================================== */
  
  std::cout << "Running test: PlayerUpdateMessage\t";
  stream={true};
  // encode
  GameOverMessage epum;
  epum.rank = 9;
  eapm.serialize(stream);
  //decode
  stream.is_writing = false;
  GameOverMessage dpum;
  dpum.serialize(stream);
  assert(epum.rank == dpum.rank);
  std::cout << "OK" << std::endl;

  /* =================================================== */

  std::cout << "All tests passed!" << std::endl;
  return 0;
}
