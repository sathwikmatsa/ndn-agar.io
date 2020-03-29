#pragma once

class Cell {
public:
    float x, y;
    int r, g, b;
    float radius;
    bool is_npc;
    virtual ~Cell() {}
    virtual float get_size() = 0;
    virtual void succumb() = 0;
};
