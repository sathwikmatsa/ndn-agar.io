#pragma once
#include "stream.hpp"

class Serialize {
  public:
    static void int_(Stream &stream, int *i, int min, int max);
    static void varint_(Stream &stream, int *i);
    static void float_(Stream &stream, float *f);
    static void uint_(Stream &stream, uint32_t *i);
    static void str_(Stream &stream, std::string &s);
};
