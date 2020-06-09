#include "serialization.hpp"
#include <cmath>
#include <cstring>
#include <limits>
#include <iostream>

using ll = long long int;
// Warning: byte-endianness is not taken into consideration

void Serialize::int_(Stream &stream, int *i, int min, int max) {
  // assert(max > min)
  ll distinct_values = (ll)max - (ll)min + 1;
  int bytes_required = std::ceil(std::log2(distinct_values) / 8);
  uint32_t value = 0;
  if(stream.is_writing) {
    value = ll(*i) - (ll)min;
    for(int b = 0; b < bytes_required; b++) {
      uint8_t byte = (value >> (8*b)) & 0xff;
      stream.data.push_back(byte);
    }
  } else {
    std::memcpy(&value, stream.data.data(), bytes_required);
    *i = (ll)value + (ll)min;
    stream.data.erase(stream.data.begin(), stream.data.begin() + bytes_required);
  }
}

void Serialize::varint_(Stream &stream, int *i) {
  Serialize::int_(stream, i, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}

void Serialize::float_(Stream &stream, float *f) {
  if(stream.is_writing) {
    auto *p = reinterpret_cast<uint8_t *>(f);
    for (int i = 0; i != sizeof(float); ++i) {
        stream.data.push_back(p[i]);
    }
  } else {
    std::memcpy(f, stream.data.data(), sizeof(float));
    stream.data.erase(stream.data.begin(), stream.data.begin() + sizeof(float));
  }
}

void Serialize::uint_(Stream &stream, uint32_t *u) {
  if(stream.is_writing) {
    auto *p = reinterpret_cast<uint8_t *>(u);
    for (int i = 0; i != sizeof(uint32_t); ++i) {
        stream.data.push_back(p[i]);
    }
  } else {
    std::memcpy(u, stream.data.data(), sizeof(uint32_t));
    stream.data.erase(stream.data.begin(), stream.data.begin() + sizeof(uint32_t));
  }
}

void Serialize::str_(Stream &stream, std::string &s) {
  if(stream.is_writing) {
    int size = s.size();
    Serialize::varint_(stream, &size);
    auto *p = reinterpret_cast<const uint8_t*>(s.data());
    for (int i = 0; i != s.size(); ++i) {
        stream.data.push_back(p[i]);
    }
  } else {
    int size;
    Serialize::varint_(stream, &size);
    s.clear();
    for(int i = 0; i < size; i++) {
      s.push_back((char)stream.data[i]);
    }
    stream.data.erase(stream.data.begin(), stream.data.begin() + size);
  }
}
