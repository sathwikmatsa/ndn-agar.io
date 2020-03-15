#pragma once
#include <string>

class Blob {
public:
    int x, y;
    void translate(int i, int j);
    bool can_eat(Blob &other_blob);
    void consume(Blob &other_blob);
    unsigned int get_radius();
    // player
    Blob(int x1, int y1, std::string name);
    // stationary blobs
    Blob(int x1, int y1);
private:
    unsigned int radius;
    bool is_npc;
    std::string player_name;
};
