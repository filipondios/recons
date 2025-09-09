#pragma once
#include <filesystem>
#include <raylib.h>
#include <raymath.h>
#include "VoxelModel.hpp"

struct ModelRender {
    // Camera settings
    float camera_speed;
    float camera_margin;
    float camera_fovy;
    Vector3 initial_view_angles;
    bool auto_rotate;
    
    // UI settings
    int base_width;
    int base_height;
    Vector2 aspect_ratio;
    int text_fontsize;
    Rectangle box;
    
    // Runtime values
    int width_scale;
    int height_scale;
    
    // 3D objects
    Camera3D camera;
    VoxelModel* model;
    Vector3 horizontal_rotation_axis;
    Vector3 vertical_rotation_axis;
    
    // Constructor and main methods
    ModelRender(VoxelModel* model);
    ~ModelRender() = default;
    void initialize();
    void render_loop();
    
private:
    void load_config(const std::filesystem::path& config_path);
    void setup_camera();
    Vector3 calculate_camera_position();   
    void rotate_horizontally(bool clockwise);
    void rotate_vertically(bool clockwise);
    void move_camera();
    void zoom();   
    void draw_help_box();
    void draw_model();
};
