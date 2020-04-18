#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <yojimbo/yojimbo.h>

struct Config {
    std::string player_name;
    yojimbo::Address server_address;
};

static const std::string help_info = R"(
Sathwik Matsa <sathwikmatsa@gmail.com>
Agario client

USAGE:
    agario_client <name> <server_address>

ARGS:
    <name>              player name, max 7 characters
    <server_address>    ip address of server [ip:port]

FLAGS:
    -h, --help      Prints help information

Example:
    $> agario_client Dhruva 127.0.0.1:9999
)";

class ArgParse {
  public:
    static Config parse_cmd(int argc, char *argv[]) {
        Config config;
        std::string ip_address;

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

        if (argc >= 3) {
            ip_address = std::string(argv[2]);
        } else {
        get_ip:
            std::cout << "enter server ip:port [eg: 127.0.0.1:9999]: ";
            std::cin >> ip_address;
        }
        config.server_address = yojimbo::Address(ip_address.c_str());
        if (!config.server_address.IsValid())
            goto get_ip;

        config.player_name = config.player_name.substr(0, 7);

        return config;
    }
};
