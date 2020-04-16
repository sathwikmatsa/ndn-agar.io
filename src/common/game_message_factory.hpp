#pragma once
#include "game_messages.hpp"
#include <yojimbo/yojimbo.h>

enum class GameMessageType { NEW_PLAYER, COUNT };

// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::NEW_PLAYER,
                             NewPlayerMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH()
