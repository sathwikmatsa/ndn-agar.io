#pragma once

#include <cstdlib>
#include <iostream>
#include <ndn-cxx/name.hpp>
#include <spdlog/spdlog.h>
#include <string>

using namespace ndn;

struct Config {
  int lobby_capacity;
};

static const std::string help_info = R"(
Sathwik Matsa <sathwikmatsa@gmail.com>
Agario server

USAGE:
    agario_server <lobby_capacity>

ARGS:
    <lobby_capacity>    lobby capacity; max players

FLAGS:
    -h, --help      Prints help information

Example:
    $> agario_server 5
)";

class ArgParse {
public:
  static Config parse_cmd(int argc, char *argv[]) {
    Config config;
    if (argc > 1) {
      config.lobby_capacity = std::atoi(argv[1]);
    } else {
      std::cout << "enter lobby capacity: ";
      std::cin >> config.lobby_capacity;
    }
    spdlog::get("flog")->info("lobby capacity: {}", config.lobby_capacity);
    return config;
  }
};
