#pragma once

#include <cstdlib>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <ndn-cxx/name.hpp>

using namespace ndn;

struct Config {
  Name server_prefix;
  int lobby_capacity;
};

static const std::string help_info = R"(
Sathwik Matsa <sathwikmatsa@gmail.com>
Agario server

USAGE:
    agario_server <server_prefix> <lobby_capacity>

ARGS:
    <server_prefix>     prefix of server
    <lobby_capacity>    lobby capacity; max players

FLAGS:
    -h, --help      Prints help information

Example:
    $> agario_server /agario/server/ 5
)";

class ArgParse {
public:
  static Config parse_cmd(int argc, char *argv[]) {
    Config config;

    if (argc >= 2) {
      if (strcmp(argv[1], "--help") == 0 or strcmp(argv[1], "-h") == 0) {
        std::cout << help_info << std::endl;
        std::exit(EXIT_SUCCESS);
      }
      config.server_prefix = std::string(argv[1]);
    } else {
      std::cout << "enter server prefix [eg: /agario/server/]: ";
      std::cin >> config.server_prefix;
    }

    if (argc >= 3) {
      config.lobby_capacity = std::atoi(argv[2]);
    } else {
      std::cout << "enter lobby capacity: ";
      std::cin >> config.lobby_capacity;
    }
    spdlog::get("flog")->info("server prefix: {}, lobby capacity: {}",
                              config.server_prefix.toUri(), config.lobby_capacity);
    return config;
  }
};
