#pragma once

inline const int PELLET_RADIUS = 20;
inline const int VIRUS_RADIUS = 110;
inline const int AGAR_RADIUS = 25;
inline const int MAX_AGAR_RADIUS = 250;
inline const int SCREEN_WIDTH = 640 * 3 / 2;
inline const int SCREEN_HEIGHT = 480 * 3 / 2;
inline const float MAX_ZOOM = 2.;
inline const int SCREEN_FPS = 60;
inline const float MAX_SPEED = 10;
inline const float MIN_SPEED = 2;
inline const int PLAYGROUND_WIDTH = 640 * 3;
inline const int PLAYGROUND_HEIGHT = 480 * 3;
inline const int GRID_TILE_SIZE = 75;
inline const int EJECTILE_INIT_VELOCITY = 500;
inline const int SPLIT_INIT_VELOCITY = 800;
inline const int DISINTEGRATE_INIT_VELOCITY = 500;
inline const int DECELERATION = 300;
inline const int EJECTILE_RADIUS = 35;
inline const int MIN_RADIUS_FOR_SPLIT = 50;
inline const int TIME_FOR_MERGE = 1000 * 20;
inline const int HUMAN_INPUT_TIMEOUT = 1000 * 3;
inline const int MAX_AGAR_COUNTERPARTS = 16;
inline const int TEXT_OUTLINE_SIZE = 2;
inline const int N_PELLETS = 100;
inline const int N_VIRUSES = 10;

inline const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;
inline const int MS_PER_UPDATE = 16;
inline const double dt = 1.0f / 60.0f;
inline const int MAX_PLAYERS = 16;
inline const char SERVER_PREFIX[] = "/ndnagario/server";
inline const char CLIENT_PREFIX[] = "/ndnagario/client";
