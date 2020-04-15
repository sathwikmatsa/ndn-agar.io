#pragma once
#include <yojimbo/yojimbo.h>
#include "game_messages.hpp"

enum class GameMessageType {
    NEW_PLAYER,
    COUNT
};

// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::NEW_PLAYER, NewPlayerMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();
