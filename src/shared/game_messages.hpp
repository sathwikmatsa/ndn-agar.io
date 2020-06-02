#pragma once
#include "game_settings.hpp"
#include "player_stats.hpp"
#include <string>
#include <tuple>
#include <vector>
#include <yojimbo/yojimbo.h>

class NewPlayerMessage : public yojimbo::Message {
public:
  int player_index;
  char player_name[8];
  uint8_t r, g, b;
  NewPlayerMessage() : player_index(-1), r(0), g(0), b(0) {
    strcpy(player_name, "unnamed");
  }

  template <typename Stream> bool Serialize(Stream &stream) {
    yojimbo_serialize_int(stream, player_index, -1, 15);
    yojimbo_serialize_string(stream, player_name, 8);
    yojimbo_serialize_int(stream, r, 0, 255);
    yojimbo_serialize_int(stream, g, 0, 255);
    yojimbo_serialize_int(stream, b, 0, 255);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class GameInfoMessage : public yojimbo::Message {
public:
  int player_index;
  std::vector<std::tuple<std::string, uint8_t, uint8_t, uint8_t>> players;
  std::vector<std::tuple<int, int, uint8_t, uint8_t, uint8_t>> pellets;
  std::vector<std::tuple<int, int>> viruses;
  GameInfoMessage() {}

  template <typename Stream> bool Serialize(Stream &stream) {
    if (Stream::IsWriting) {
      yojimbo_serialize_int(stream, player_index, 0, 15);
      int n_players = players.size();
      yojimbo_serialize_int(stream, n_players, 0, 16);
      for (auto &player : players) {
        std::string name = std::get<0>(player);
        char name_str[8];
        strcpy(name_str, name.c_str());
        yojimbo_serialize_string(stream, name_str, 8);
        yojimbo_serialize_int(stream, std::get<1>(player), 0, 255);
        yojimbo_serialize_int(stream, std::get<2>(player), 0, 255);
        yojimbo_serialize_int(stream, std::get<3>(player), 0, 255);
      }
      int n_pellets = pellets.size();
      yojimbo_serialize_int(stream, n_pellets, 0, 255);
      for (auto &pellet : pellets) {
        yojimbo_serialize_varint32(stream, std::get<0>(pellet));
        yojimbo_serialize_varint32(stream, std::get<1>(pellet));
        yojimbo_serialize_int(stream, std::get<2>(pellet), 0, 255);
        yojimbo_serialize_int(stream, std::get<3>(pellet), 0, 255);
        yojimbo_serialize_int(stream, std::get<4>(pellet), 0, 255);
      }
      int n_viruses = viruses.size();
      yojimbo_serialize_int(stream, n_viruses, 0, 15);
      for (auto &virus : viruses) {
        yojimbo_serialize_varint32(stream, std::get<0>(virus));
        yojimbo_serialize_varint32(stream, std::get<1>(virus));
      }
    } else {
      yojimbo_serialize_int(stream, player_index, 0, 15);
      int n_players;
      yojimbo_serialize_int(stream, n_players, 0, 16);
      players.clear();
      for (int i = 0; i < n_players; i++) {
        char name[8];
        uint8_t r, g, b;
        yojimbo_serialize_string(stream, name, 8);
        yojimbo_serialize_int(stream, r, 0, 255);
        yojimbo_serialize_int(stream, g, 0, 255);
        yojimbo_serialize_int(stream, b, 0, 255);
        players.push_back(std::make_tuple(std::string(name), r, g, b));
      }
      int n_pellets;
      yojimbo_serialize_int(stream, n_pellets, 0, 255);
      pellets.clear();
      for (int i = 0; i < n_pellets; i++) {
        int x, y;
        uint8_t r, g, b;
        yojimbo_serialize_varint32(stream, x);
        yojimbo_serialize_varint32(stream, y);
        yojimbo_serialize_int(stream, r, 0, 255);
        yojimbo_serialize_int(stream, g, 0, 255);
        yojimbo_serialize_int(stream, b, 0, 255);
        pellets.push_back(std::make_tuple(x, y, r, g, b));
      }
      int n_viruses;
      yojimbo_serialize_int(stream, n_viruses, 0, 15);
      viruses.clear();
      for (int i = 0; i < n_viruses; i++) {
        int x, y;
        yojimbo_serialize_varint32(stream, x);
        yojimbo_serialize_varint32(stream, y);
        viruses.push_back(std::make_tuple(x, y));
      }
    }
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class AtePelletMessage : public yojimbo::Message {
public:
  int pellet_id;
  AtePelletMessage() : pellet_id(0) {}

  template <typename Stream> bool Serialize(Stream &stream) {
    yojimbo_serialize_int(stream, pellet_id, 0, 255);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class PelletRelocMessage : public yojimbo::Message {
public:
  int pellet_id;
  int pos_x, pos_y;
  PelletRelocMessage() : pellet_id(0), pos_x(0), pos_y(0) {}

  template <typename Stream> bool Serialize(Stream &stream) {
    yojimbo_serialize_int(stream, pellet_id, 0, 255);
    yojimbo_serialize_varint32(stream, pos_x);
    yojimbo_serialize_varint32(stream, pos_y);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class GameOverMessage : public yojimbo::Message {
public:
  int rank;
  GameOverMessage() : rank(0) {}

  template <typename Stream> bool Serialize(Stream &stream) {
    yojimbo_serialize_int(stream, rank, 0, 16);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class PlayerUpdateMessage : public yojimbo::Message {
public:
  uint32_t seq_id;
  PlayerStats info;
  PlayerUpdateMessage() {}

  template <typename Stream> bool Serialize(Stream &stream) {
    if (Stream::IsWriting) {
      yojimbo_serialize_uint32(stream, seq_id);
      // write cells
      int n_cells = info.cells.size();
      yojimbo_serialize_int(stream, n_cells, 0, MAX_AGAR_COUNTERPARTS);
      for (auto &cell : info.cells) {
        auto [x, y, r] = cell;
        yojimbo_serialize_float(stream, x);
        yojimbo_serialize_float(stream, y);
        yojimbo_serialize_float(stream, r);
      }
      // write ejectiles
      int n_ejectiles = info.ejectiles.size();
      yojimbo_serialize_varint32(stream, n_ejectiles);
      for (auto &ejectile : info.ejectiles) {
        auto [x, y] = ejectile;
        yojimbo_serialize_float(stream, x);
        yojimbo_serialize_float(stream, y);
      }
    } else {
      yojimbo_serialize_uint32(stream, seq_id);
      info.clear();
      // read cells
      int n_cells;
      yojimbo_serialize_int(stream, n_cells, 0, MAX_AGAR_COUNTERPARTS);
      for (int i = 0; i < n_cells; i++) {
        float x, y, r;
        yojimbo_serialize_float(stream, x);
        yojimbo_serialize_float(stream, y);
        yojimbo_serialize_float(stream, r);
        info.cells.emplace_back(x, y, r);
      }
      // read ejectiles
      int n_ejectiles;
      yojimbo_serialize_varint32(stream, n_ejectiles);
      for (int i = 0; i < n_ejectiles; i++) {
        float x, y;
        yojimbo_serialize_float(stream, x);
        yojimbo_serialize_float(stream, y);
        info.ejectiles.emplace_back(x, y);
      }
    }
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class SnapshotMessage : public yojimbo::Message {
public:
  uint32_t id;
  std::vector<PlayerStats> stats;
  SnapshotMessage() {}

  template <typename Stream> bool Serialize(Stream &stream) {
    if (Stream::IsWriting) {
      yojimbo_serialize_uint32(stream, id);
      // write players stats
      int n_players = stats.size();
      yojimbo_serialize_int(stream, n_players, 0, MAX_PLAYERS);
      for (auto &stat : stats) {
        // write cells
        int n_cells = stat.cells.size();
        yojimbo_serialize_int(stream, n_cells, 0, MAX_AGAR_COUNTERPARTS);
        for (auto &cell : stat.cells) {
          auto [x, y, r] = cell;
          yojimbo_serialize_float(stream, x);
          yojimbo_serialize_float(stream, y);
          yojimbo_serialize_float(stream, r);
        }
        // write ejectiles
        int n_ejectiles = stat.ejectiles.size();
        yojimbo_serialize_varint32(stream, n_ejectiles);
        for (auto &ejectile : stat.ejectiles) {
          auto [x, y] = ejectile;
          yojimbo_serialize_float(stream, x);
          yojimbo_serialize_float(stream, y);
        }
      }
    } else {
      yojimbo_serialize_uint32(stream, id);
      stats.clear();
      // read players
      int n_players;
      yojimbo_serialize_int(stream, n_players, 0, MAX_PLAYERS);
      for (int k = 0; k < n_players; k++) {
        PlayerStats info;
        // read cells
        int n_cells;
        yojimbo_serialize_int(stream, n_cells, 0, MAX_AGAR_COUNTERPARTS);
        for (int i = 0; i < n_cells; i++) {
          float x, y, r;
          yojimbo_serialize_float(stream, x);
          yojimbo_serialize_float(stream, y);
          yojimbo_serialize_float(stream, r);
          info.cells.emplace_back(x, y, r);
        }
        // read ejectiles
        int n_ejectiles;
        yojimbo_serialize_varint32(stream, n_ejectiles);
        for (int i = 0; i < n_ejectiles; i++) {
          float x, y;
          yojimbo_serialize_float(stream, x);
          yojimbo_serialize_float(stream, y);
          info.ejectiles.emplace_back(x, y);
        }
        stats.push_back(info);
      }
    }
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class DeadPlayerMessage : public yojimbo::Message {
public:
  int player_index;
  DeadPlayerMessage() : player_index(-1) {}

  template <typename Stream> bool Serialize(Stream &stream) {
    yojimbo_serialize_int(stream, player_index, -1, 15);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};
