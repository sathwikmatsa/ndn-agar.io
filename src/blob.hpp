#pragma once
#include <string>
#include "context.hpp"

class Blob {
public:
    float x, y;
    void follow_mouse(int x1, int y1, Camera& camera);
    bool can_eat(Blob &other_blob);
    void consume(Blob &other_blob);
    float get_radius();
    void succumb();
    // player
    Blob(int x1, int y1, std::string name);
    // stationary blobs
    Blob(int x1, int y1);
private:
    float radius;
    bool is_npc;
    std::string player_name;
};
