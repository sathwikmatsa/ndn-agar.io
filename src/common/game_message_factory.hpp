#pragma once
#include "game_messages.hpp"
#include <yojimbo/yojimbo.h>

enum class GameMessageType {
  NEW_PLAYER,
  GAME_INFO,
  ATE_PELLET,
  PELLET_RELOC,
  PLAYER_UPDATE,
  SNAPSHOT,
  DEAD_PLAYER,
  GAME_OVER,
  COUNT
};

// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::NEW_PLAYER,
                             NewPlayerMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::GAME_INFO, GameInfoMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::ATE_PELLET,
                             AtePelletMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::PELLET_RELOC,
                             PelletRelocMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::PLAYER_UPDATE,
                             PlayerUpdateMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::DEAD_PLAYER,
                             DeadPlayerMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::SNAPSHOT, SnapshotMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::GAME_OVER, GameOverMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH()
