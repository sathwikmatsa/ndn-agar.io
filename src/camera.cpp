#include "camera.hpp"
#include "game_settings.hpp"

Camera::Camera() {
    width = SCREEN_WIDTH;
    height = SCREEN_HEIGHT;
    camera = {0, 0, width, height};
}

bool Camera::at_boundary() {
    return camera_at_boundary;
}

int Camera::x_offset() {
    return camera.x - width/2;
}

int Camera::y_offset() {
    return camera.y - height/2;
}

void Camera::set_center(int cx, int cy) {
    camera.x = cx;
    camera.y = cy;
    camera_at_boundary = false;

    if(camera.x < width/2) {
        camera.x = width/2;
        camera_at_boundary = true;
    }
    if(camera.y < height/2) {
        camera.y = height/2;
        camera_at_boundary = true;
    }
    if(camera.x > (PLAYGROUND_WIDTH - width/2)) {
        camera.x = PLAYGROUND_WIDTH - width/2;
        camera_at_boundary = true;
    }
    if(camera.y > (PLAYGROUND_HEIGHT - height/2)) {
        camera.y = PLAYGROUND_HEIGHT - height/2;
        camera_at_boundary = true;
    }
}
