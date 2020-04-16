#pragma once
#include <string>
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
