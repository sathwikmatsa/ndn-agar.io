#pragma once
#include <string>

class Blob {
public:
    int x, y;
    void follow_mouse(int x1, int y1);
    bool can_eat(Blob &other_blob);
    void consume(Blob &other_blob);
    int get_radius();
    // player
    Blob(int x1, int y1, std::string name);
    // stationary blobs
    Blob(int x1, int y1);
private:
    int radius;
    bool is_npc;
    std::string player_name;
};
