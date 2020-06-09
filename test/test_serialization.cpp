#include "./../src/shared/serialization.hpp"
#include "./../src/shared/stream.hpp"
#include <iostream>
#include <cassert>

int main() {
  Stream stream = {true};
  // encode
  // int
  int edata1 = -345;
  int edata2 = -3;
  int edata3 = 0;
  int edata4 = 23;
  int edata5 = 450;
  // varint
  int edata6 = 422222;
  int edata7 = -422222;
  // float
  float edata8 = -87.543;
  float edata9 = 1287.543;
  // uint32
  uint32_t edata10 = 4294967295;
  // string
  std::string edata11("Dhruva");

  Serialize::int_(stream, &edata1, -345, 500);
  Serialize::int_(stream, &edata2, -350, 500);
  Serialize::int_(stream, &edata3, -350, 500);
  Serialize::int_(stream, &edata4, -350, 500);
  Serialize::int_(stream, &edata5, -350, 450);
  Serialize::varint_(stream, &edata6);
  Serialize::varint_(stream, &edata7);
  Serialize::float_(stream, &edata8);
  Serialize::float_(stream, &edata9);
  Serialize::uint_(stream, &edata10);
  Serialize::str_(stream, edata11);

  // decode
  stream.is_writing = false;
  int ddata1, ddata2, ddata3, ddata4, ddata5;
  int ddata6, ddata7;
  float ddata8, ddata9;
  uint32_t ddata10;
  std::string ddata11;
  Serialize::int_(stream, &ddata1, -345, 500);
  Serialize::int_(stream, &ddata2, -350, 500);
  Serialize::int_(stream, &ddata3, -350, 500);
  Serialize::int_(stream, &ddata4, -350, 500);
  Serialize::int_(stream, &ddata5, -350, 450);
  Serialize::varint_(stream, &ddata6);
  Serialize::varint_(stream, &ddata7);
  Serialize::float_(stream, &ddata8);
  Serialize::float_(stream, &ddata9);
  Serialize::uint_(stream, &ddata10);
  Serialize::str_(stream, ddata11);

  // compare
  assert(edata1 == ddata1);
  assert(edata2 == ddata2);
  assert(edata3 == ddata3);
  assert(edata4 == ddata4);
  assert(edata5 == ddata5);
  assert(edata6 == ddata6);
  assert(edata7 == ddata7);
  assert(edata8 == ddata8);
  assert(edata9 == ddata9);
  assert(edata10 == ddata10);
  assert(edata11.compare(ddata11) == 0);

  std::cout << "All tests passed!" << std::endl;
  return 0;
}
