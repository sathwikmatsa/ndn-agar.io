#pragma once

// note: these are not global variables..each translation unit will have a separate copy of these variables..
// since these are just constants and won't be modified during runtime..this works just fine
const int PELLET_RADIUS = 20;
const int VIRUS_RADIUS = 110;
const int AGAR_RADIUS = 25;
const int MAX_AGAR_RADIUS = 250;
const int SCREEN_WIDTH = 640 * 3 / 2;
const int SCREEN_HEIGHT = 480 * 3 / 2;
const float MAX_ZOOM = 2.;
const int SCREEN_FPS = 60;
const float MAX_SPEED = 10;
const float MIN_SPEED = 2;
const int PLAYGROUND_WIDTH = 640 * 3;
const int PLAYGROUND_HEIGHT = 480 * 3;
const int GRID_TILE_SIZE = 75;
const int EJECTILE_INIT_VELOCITY = 500;
const int SPLIT_INIT_VELOCITY = 800;
const int DISINTEGRATE_INIT_VELOCITY = 500;
const int DECELERATION = 300;
const int EJECTILE_RADIUS = 35;
const int MIN_RADIUS_FOR_SPLIT = 50;
const int TIME_FOR_MERGE = 1000 * 20;
const int HUMAN_INPUT_TIMEOUT = 1000 * 3;
const int MAX_AGAR_COUNTERPARTS = 16;
const int TEXT_OUTLINE_SIZE = 2;
const int N_PELLETS = 100;
const int N_VIRUSES = 10;

const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;
const int MS_PER_UPDATE = 16;
const double dt = 1.0f / 60.0f;
const int MAX_PLAYERS = 16;
const char SERVER_PREFIX[] = "/ndnagario/server";
const char CLIENT_PREFIX[] = "/ndnagario/client";
