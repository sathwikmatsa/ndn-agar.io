#pragma once
#include <SDL2/SDL.h>

class Camera {
public:
    Camera();
    int x_offset();
    int y_offset();
    bool at_boundary();
    void set_center(int cx, int cy);
private:
    int width;
    int height;
    bool camera_at_boundary;
    SDL_Rect camera;
};
