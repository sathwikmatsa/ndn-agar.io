#include "camera.hpp"
#include "game_settings.hpp"
#include <SDL2/SDL.h>

Camera::Camera() {
    current_scale = 1;
    camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
}

bool Camera::at_boundary() {
    return camera_at_boundary;
}

int Camera::x_offset() {
    return camera.x - camera.w/2;
}

int Camera::y_offset() {
    return camera.y - camera.h/2;
}

void Camera::scale(float scale_factor, SDL_Renderer* renderer) {
    int new_width = SCREEN_WIDTH * scale_factor;
    int new_height = SCREEN_HEIGHT * scale_factor;

    SDL_RenderSetLogicalSize(
        renderer,
        new_width,
        new_height
    );

    camera.w = new_width;
    camera.h = new_height;
    current_scale = scale_factor;
}

void Camera::set_center(int cx, int cy) {
    camera.x = cx;
    camera.y = cy;
    camera_at_boundary = false;

    if(camera.x < camera.w/2) {
        camera.x = camera.w/2;
        camera_at_boundary = true;
    }
    if(camera.y < camera.h/2) {
        camera.y = camera.h/2;
        camera_at_boundary = true;
    }
    if(camera.x > (PLAYGROUND_WIDTH - camera.w/2)) {
        camera.x = PLAYGROUND_WIDTH - camera.w/2;
        camera_at_boundary = true;
    }
    if(camera.y > (PLAYGROUND_HEIGHT - camera.h/2)) {
        camera.y = PLAYGROUND_HEIGHT - camera.h/2;
        camera_at_boundary = true;
    }
}
