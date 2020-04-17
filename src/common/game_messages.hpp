#pragma once
#include <string>
#include <tuple>
#include <vector>
#include <yojimbo/yojimbo.h>

class NewPlayerMessage : public yojimbo::Message {
  public:
    char player_name[8];
    uint8_t r, g, b;
    NewPlayerMessage() : r(0), g(0), b(0) { strcpy(player_name, "unnamed"); }

    template <typename Stream> bool Serialize(Stream &stream) {
        serialize_string(stream, player_name, 8);
        serialize_int(stream, r, 0, 255);
        serialize_int(stream, g, 0, 255);
        serialize_int(stream, b, 0, 255);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class PelletsInfoMessage : public yojimbo::Message {
  public:
    std::vector<std::tuple<int, int, uint8_t, uint8_t, uint8_t>> pellets;
    PelletsInfoMessage() {}

    template <typename Stream> bool Serialize(Stream &stream) {
        if (Stream::IsWriting) {
            int n_pellets = pellets.size();
            serialize_int(stream, n_pellets, 0, 255);
            for (auto &pellet : pellets) {
                serialize_varint32(stream, std::get<0>(pellet));
                serialize_varint32(stream, std::get<1>(pellet));
                serialize_int(stream, std::get<2>(pellet), 0, 255);
                serialize_int(stream, std::get<3>(pellet), 0, 255);
                serialize_int(stream, std::get<4>(pellet), 0, 255);
            }
        } else {
            int n_pellets;
            serialize_int(stream, n_pellets, 0, 255);
            pellets.clear();
            for (int i = 0; i < n_pellets; i++) {
                int x, y;
                uint8_t r, g, b;
                serialize_varint32(stream, x);
                serialize_varint32(stream, y);
                serialize_int(stream, r, 0, 255);
                serialize_int(stream, g, 0, 255);
                serialize_int(stream, b, 0, 255);
                pellets.push_back(std::make_tuple(x, y, r, g, b));
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
        serialize_int(stream, pellet_id, 0, 255);
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
        serialize_int(stream, pellet_id, 0, 255);
        serialize_varint32(stream, pos_x);
        serialize_varint32(stream, pos_y);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};

class GameOverMessage : public yojimbo::Message {
  public:
    bool gameover;
    GameOverMessage() : gameover(true) {}

    template <typename Stream> bool Serialize(Stream &stream) {
        serialize_bool(stream, gameover);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
};
