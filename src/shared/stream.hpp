#pragma once
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>

struct Stream {
  bool is_writing;
  std::vector<uint8_t> data;

  void debug() {
    std::for_each(data.begin(), data.end(), [](uint8_t b) {std::cout << (int)b << " ";});
    std::cout << std::endl;
  }
};

