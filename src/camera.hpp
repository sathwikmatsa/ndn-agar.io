#pragma once
#include <SDL2/SDL.h>

class Camera {
public:
    Camera();
    int x_offset();
    int y_offset();
    int width();
    int height();
    float current_scale;
    void scale(float scale_factor, SDL_Renderer* renderer);
    bool at_boundary();
    void set_center(int cx, int cy);
private:
    bool camera_at_boundary;
    SDL_Rect camera;
};
