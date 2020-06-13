#pragma once
#include "game_settings.hpp"
#include "player_stats.hpp"
#include "serialization.hpp"
#include <string>
#include <tuple>
#include <vector>

enum class GameMessageType {
  NEW_PLAYER,
  ATE_PELLET,
  PLAYER_UPDATE,
  GAME_INFO,
  PELLET_RELOC,
  SNAPSHOT,
  DEAD_PLAYER,
  GAME_OVER,
  COUNT
};

class NdnAgarioMessage {
public:
  virtual ~NdnAgarioMessage() {}
  virtual void serialize(Stream &stream) = 0;
  virtual int id() = 0;
};

class NewPlayerMessage : public NdnAgarioMessage {
public:
  int player_index;
  int r, g, b;
  std::string player_name;
  NewPlayerMessage()
      : player_index(-1), r(0), g(0), b(0), player_name("unnamed") {}

  void serialize(Stream &stream) override {
    Serialize::int_(stream, &player_index, -1, 15);
    Serialize::str_(stream, player_name);
    Serialize::int_(stream, &r, 0, 255);
    Serialize::int_(stream, &g, 0, 255);
    Serialize::int_(stream, &b, 0, 255);
  }

  int id() override { return (int)GameMessageType::NEW_PLAYER; }
};

class GameInfoMessage : public NdnAgarioMessage {
public:
  int player_index;
  std::vector<std::tuple<std::string, uint8_t, uint8_t, uint8_t>> players;
  std::vector<std::tuple<int, int, uint8_t, uint8_t, uint8_t>> pellets;
  std::vector<std::tuple<int, int>> viruses;
  GameInfoMessage() {}

  void serialize(Stream &stream) override {
    if (stream.is_writing) {
      int r, g, b;
      int x, y;
      Serialize::int_(stream, &player_index, 0, 15);
      int n_players = players.size();
      Serialize::int_(stream, &n_players, 0, 16);
      for (auto &player : players) {
        std::string name = std::get<0>(player);
        Serialize::str_(stream, name);
        r = std::get<1>(player);
        g = std::get<2>(player);
        b = std::get<3>(player);
        Serialize::int_(stream, &r, 0, 255);
        Serialize::int_(stream, &g, 0, 255);
        Serialize::int_(stream, &b, 0, 255);
      }
      int n_pellets = pellets.size();
      Serialize::int_(stream, &n_pellets, 0, 255);
      for (auto &pellet : pellets) {
        x = std::get<0>(pellet);
        y = std::get<1>(pellet);
        Serialize::varint_(stream, &x);
        Serialize::varint_(stream, &y);
        r = std::get<2>(pellet);
        g = std::get<3>(pellet);
        b = std::get<4>(pellet);
        Serialize::int_(stream, &r, 0, 255);
        Serialize::int_(stream, &g, 0, 255);
        Serialize::int_(stream, &b, 0, 255);
      }
      int n_viruses = viruses.size();
      Serialize::int_(stream, &n_viruses, 0, 15);
      for (auto &virus : viruses) {
        x = std::get<0>(virus);
        y = std::get<1>(virus);
        Serialize::varint_(stream, &x);
        Serialize::varint_(stream, &y);
      }
    } else {
      int r, g, b;
      int x, y;
      Serialize::int_(stream, &player_index, 0, 15);
      int n_players;
      Serialize::int_(stream, &n_players, 0, 16);
      players.clear();
      for (int i = 0; i < n_players; i++) {
        std::string name;
        Serialize::str_(stream, name);
        Serialize::int_(stream, &r, 0, 255);
        Serialize::int_(stream, &g, 0, 255);
        Serialize::int_(stream, &b, 0, 255);
        players.push_back(std::make_tuple(name, r, g, b));
      }
      int n_pellets;
      Serialize::int_(stream, &n_pellets, 0, 255);
      pellets.clear();
      for (int i = 0; i < n_pellets; i++) {
        Serialize::varint_(stream, &x);
        Serialize::varint_(stream, &y);
        Serialize::int_(stream, &r, 0, 255);
        Serialize::int_(stream, &g, 0, 255);
        Serialize::int_(stream, &b, 0, 255);
        pellets.push_back(std::make_tuple(x, y, r, g, b));
      }
      int n_viruses;
      Serialize::int_(stream, &n_viruses, 0, 15);
      viruses.clear();
      for (int i = 0; i < n_viruses; i++) {
        Serialize::varint_(stream, &x);
        Serialize::varint_(stream, &y);
        viruses.push_back(std::make_tuple(x, y));
      }
    }
  }

  int id() override { return (int)GameMessageType::GAME_INFO; }
};

class AtePelletMessage : public NdnAgarioMessage {
public:
  int pellet_id;
  AtePelletMessage() : pellet_id(0) {}

  void serialize(Stream &stream) override {
    Serialize::int_(stream, &pellet_id, 0, 255);
  }

  int id() override { return (int)GameMessageType::ATE_PELLET; }
};

class PelletRelocMessage : public NdnAgarioMessage {
public:
  int pellet_id;
  int pos_x, pos_y;
  PelletRelocMessage() : pellet_id(0), pos_x(0), pos_y(0) {}

  void serialize(Stream &stream) override {
    Serialize::int_(stream, &pellet_id, 0, 255);
    Serialize::varint_(stream, &pos_x);
    Serialize::varint_(stream, &pos_y);
  }

  int id() override { return (int)GameMessageType::PELLET_RELOC; }
};

class GameOverMessage : public NdnAgarioMessage {
public:
  int rank;
  GameOverMessage() : rank(0) {}

  void serialize(Stream &stream) override {
    Serialize::int_(stream, &rank, 0, 16);
  }

  int id() override { return (int)GameMessageType::GAME_OVER; }
};

class PlayerUpdateMessage : public NdnAgarioMessage {
public:
  uint32_t seq_id;
  PlayerStats info;
  PlayerUpdateMessage() {}

  void serialize(Stream &stream) override {
    if (stream.is_writing) {
      Serialize::uint_(stream, &seq_id);
      // write cells
      int n_cells = info.cells.size();
      Serialize::int_(stream, &n_cells, 0, MAX_AGAR_COUNTERPARTS);
      for (auto &cell : info.cells) {
        auto [x, y, r] = cell;
        Serialize::float_(stream, &x);
        Serialize::float_(stream, &y);
        Serialize::float_(stream, &r);
      }
      // write ejectiles
      int n_ejectiles = info.ejectiles.size();
      Serialize::varint_(stream, &n_ejectiles);
      for (auto &ejectile : info.ejectiles) {
        auto [x, y] = ejectile;
        Serialize::float_(stream, &x);
        Serialize::float_(stream, &y);
      }
    } else {
      Serialize::uint_(stream, &seq_id);
      info.clear();
      // read cells
      int n_cells;
      Serialize::int_(stream, &n_cells, 0, MAX_AGAR_COUNTERPARTS);
      for (int i = 0; i < n_cells; i++) {
        float x, y, r;
        Serialize::float_(stream, &x);
        Serialize::float_(stream, &y);
        Serialize::float_(stream, &r);
        info.cells.emplace_back(x, y, r);
      }
      // read ejectiles
      int n_ejectiles;
      Serialize::varint_(stream, &n_ejectiles);
      for (int i = 0; i < n_ejectiles; i++) {
        float x, y;
        Serialize::float_(stream, &x);
        Serialize::float_(stream, &y);
        info.ejectiles.emplace_back(x, y);
      }
    }
  }

  int id() override { return (int)GameMessageType::PLAYER_UPDATE; }
};

class SnapshotMessage : public NdnAgarioMessage {
public:
  uint32_t seq_id;
  std::vector<PlayerStats> stats;
  SnapshotMessage() {}

  void serialize(Stream &stream) override {
    if (stream.is_writing) {
      Serialize::uint_(stream, &seq_id);
      // write players stats
      int n_players = stats.size();
      Serialize::int_(stream, &n_players, 0, MAX_PLAYERS);
      for (auto &stat : stats) {
        // write cells
        int n_cells = stat.cells.size();
        Serialize::int_(stream, &n_cells, 0, MAX_AGAR_COUNTERPARTS);
        for (auto &cell : stat.cells) {
          auto [x, y, r] = cell;
          Serialize::float_(stream, &x);
          Serialize::float_(stream, &y);
          Serialize::float_(stream, &r);
        }
        // write ejectiles
        int n_ejectiles = stat.ejectiles.size();
        Serialize::varint_(stream, &n_ejectiles);
        for (auto &ejectile : stat.ejectiles) {
          auto [x, y] = ejectile;
          Serialize::float_(stream, &x);
          Serialize::float_(stream, &y);
        }
      }
    } else {
      Serialize::uint_(stream, &seq_id);
      stats.clear();
      // read players
      int n_players;
      Serialize::int_(stream, &n_players, 0, MAX_PLAYERS);
      for (int k = 0; k < n_players; k++) {
        PlayerStats info;
        // read cells
        int n_cells;
        Serialize::int_(stream, &n_cells, 0, MAX_AGAR_COUNTERPARTS);
        for (int i = 0; i < n_cells; i++) {
          float x, y, r;
          Serialize::float_(stream, &x);
          Serialize::float_(stream, &y);
          Serialize::float_(stream, &r);
          info.cells.emplace_back(x, y, r);
        }
        // read ejectiles
        int n_ejectiles;
        Serialize::varint_(stream, &n_ejectiles);
        for (int i = 0; i < n_ejectiles; i++) {
          float x, y;
          Serialize::float_(stream, &x);
          Serialize::float_(stream, &y);
          info.ejectiles.emplace_back(x, y);
        }
        stats.push_back(info);
      }
    }
  }

  int id() override { return (int)GameMessageType::SNAPSHOT; }
};

class DeadPlayerMessage : public NdnAgarioMessage {
public:
  int player_index;
  DeadPlayerMessage() : player_index(-1) {}

  void serialize(Stream &stream) override {
    Serialize::int_(stream, &player_index, -1, 15);
  }

  int id() override { return (int)GameMessageType::DEAD_PLAYER; }
};
