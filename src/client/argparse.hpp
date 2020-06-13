#pragma once
#include "bot.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

struct Config {
  std::string player_name;
  std::unique_ptr<Bot> bot;
  Config() : bot(nullptr) {}
};

static const std::string help_info = R"(
Sathwik Matsa <sathwikmatsa@gmail.com>
Agario client

USAGE:
    agario_client <name> enable:Goblin

ARGS:
    <name>              player name, max 7 characters
    enable:Goblin       enables bot behaviour (for testing purposes)

FLAGS:
    -h, --help      Prints help information

Example:
    $> agario_client Dhruva enable:Goblin
)";

class ArgParse {
public:
  static Config parse_cmd(int argc, char *argv[]) {
    Config config;
    std::string server_prefix;

    if (argc >= 2) {
      if (strcmp(argv[1], "--help") == 0 or strcmp(argv[1], "-h") == 0) {
        std::cout << help_info << std::endl;
        std::exit(EXIT_SUCCESS);
      }
      config.player_name = std::string(argv[1]);
    } else {
      std::cout << "enter player name [max 7 characters]: ";
      std::cin >> config.player_name;
    }

    config.player_name = config.player_name.substr(0, 7);

    if (argc > 2) {
      std::string bot_arg = std::string(argv[2]);
      if (bot_arg.substr(0, 7) == "enable:") {
        std::string bot_type = bot_arg.substr(7);
        if (bot_type == "Goblin") {
          config.bot = std::unique_ptr<Bot>(new Goblin());
          spdlog::info("Goblin enabled");
        }
      }
    }

    spdlog::get("flog")->info("name: {}, is_bot_enabled: {}",
                              config.player_name, config.bot != nullptr);
    return config;
  }
};
