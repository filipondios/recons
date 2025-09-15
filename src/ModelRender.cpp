#include "ModelRender.hpp"
#include "raymath.h"
#define RADIANS(deg) (deg * M_PI / 180.0f)


ModelRender::ModelRender(const VoxelModel* model):
    model(model),
    // TODO store defs in json!
    camera_speed(0.02f),
    camera_margin(-0.1),
    camera_fovy(60),
    initial_view_angles{45, 45, 45},
    auto_rotate(true),
    base_width(1366),
    base_height(768),
    aspect_ratio{16, 9},
    text_fontsize(20),
    box{10, 10, 500, 100} {

    // Setup the rendering camera details
    this->camera = {
        .position = this->calculate_camera_position(),
        .target = {0, 0, 0},
        .up = {0, 1, 0},
        .fovy = camera_fovy,
        .projection = CAMERA_PERSPECTIVE,
    };

    this->horizontal_rotation_axis = Vector3Normalize({
        this->camera.position.z, 0, -this->camera.position.x});
    this->vertical_rotation_axis = this->camera.up;
}

Vector3 ModelRender::calculate_camera_position(void) const {
    // Computes camera position to fit the model

    const Vector3 center {
        // x = (min_x + max_x) / 2
        // y = (min_y + max_y) / 2
        // z = (min_z + max_z) / 2
        (this->model->bounds[0] + this->model->bounds[1]) / 2.0f,
        (this->model->bounds[2] + this->model->bounds[3]) / 2.0f,
        (this->model->bounds[4] + this->model->bounds[5]) / 2.0f,
    };

    const float rx = (model->bounds[1] - model->bounds[0]) / 2.0f;
    const float ry = (model->bounds[3] - model->bounds[2]) / 2.0f;
    const float rz = (model->bounds[5] - model->bounds[4]) / 2.0f;
    const float radius = std::sqrt(rx * rx + ry * ry + rz * rz);

    const float ratio = this->aspect_ratio[0] / this->aspect_ratio[1];
    const float fov_y_radians = RADIANS(this->camera_fovy);
    float fov_x_radians = std::tan(fov_y_radians / 2.0f);
    fov_x_radians = 2.0f * std::atan(fov_x_radians) * ratio;

    const float limiting_fov = std::min(fov_x_radians, fov_y_radians);
    float distance = radius / std::sin(limiting_fov / 2.0f);
    distance *= (1 + this->camera_margin);

    const float angle_x_rad = RADIANS(initial_view_angles[0]);
    const float angle_y_rad = RADIANS(initial_view_angles[1]);
    const float angle_z_rad = RADIANS(initial_view_angles[2]);

   return Vector3Add({
        // camera offsets {x, y, z}
        distance * std::sin(angle_x_rad),
        distance * std::sin(angle_y_rad),
        distance * std::sin(angle_z_rad),
    }, center);
}

void ModelRender::rotate_horizontally(bool clockwise) {
    // Rotates the camera arround the vertical axis
    const float speed = clockwise? this->camera_speed : -this->camera_speed;

    this->camera.position = Vector3RotateByAxisAngle(this->camera.position,
        this->vertical_rotation_axis, speed);
    this->horizontal_rotation_axis = Vector3RotateByAxisAngle(
        this->horizontal_rotation_axis, this->vertical_rotation_axis, speed);
}


void ModelRender::rotate_vertically(bool clockwise) {
    // Rotates the camera arround an axis in the XZ plane
    const float speed = clockwise? this->camera_speed : -this->camera_speed;

    this->camera.position = Vector3RotateByAxisAngle(this->camera.position,
        this->horizontal_rotation_axis, speed);
}

void ModelRender::move_camera(void) {
    // Rotates the camera automatically if auto-rotate 
    // is enabled, else checks for user input.   
}

void ModelRender::initialize_render_context(void) {
    // Initializes the Raylib 3D context    
}

void ModelRender::zoom(void) {
    // zooms the camera in or out based on mouse wheel
    // movement. If there is no movement, the camera position
    // remains unchanged.
}

void ModelRender::draw_help_box(void) const {
    // Draws the help box with instructions for the user.
}

void ModelRender::start_render_loop() {
    // Draws the reconstructed model
}
