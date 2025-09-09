#include <nlohmann/json.hpp>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "ModelRender.hpp"

ModelRender::ModelRender(VoxelModel* model) : model(model) {
    if (!model) {
        throw std::runtime_error("Model cannot be null");
    }
    
    load_config(std::filesystem::path("config") / "render.json");
    setup_camera();
}

void ModelRender::load_config(const std::filesystem::path& config_path) {
    if (!std::filesystem::exists(config_path)) {
        throw std::runtime_error("Config file not found: " + config_path.string());
    }
    
    std::ifstream file(config_path);
    nlohmann::json data;
    file >> data;
    
    // Parse camera config
    auto camera_config = data["camera"];
    camera_speed = camera_config["speed"];
    camera_margin = camera_config["margin"];
    camera_fovy = camera_config["fovy"];
    auto_rotate = camera_config["auto_rotate"];
    
    auto angles = camera_config["initial_angles"];
    initial_view_angles = {
        angles["x"].get<float>(),
        angles["y"].get<float>(),
        angles["z"].get<float>()
    };
    
    // Parse UI config
    auto ui_base = data["ui_base"];
    base_width = ui_base["canvas_width"];
    base_height = ui_base["canvas_height"];
    
    auto aspect_ratio_config = ui_base["aspect_ratio"];
    aspect_ratio = {
        aspect_ratio_config["width"].get<float>(),
        aspect_ratio_config["height"].get<float>()
    };
    
    text_fontsize = ui_base["font_size"];   
    auto textbox = ui_base["textbox_rectangle"];
    box = {
        textbox[0].get<float>(),
        textbox[1].get<float>(),
        textbox[2].get<float>(),
        textbox[3].get<float>()
    };
}

void ModelRender::setup_camera() {
    Vector3 position = calculate_camera_position();
    Vector3 target = {0.0f, 0.0f, 0.0f};
    Vector3 up = {0.0f, 1.0f, 0.0f};
    
    camera = {
        .position = position,
        .target = target,
        .up = up,
        .fovy = camera_fovy,
        .projection = CAMERA_PERSPECTIVE
    };
    
    horizontal_rotation_axis = Vector3Normalize({position.z, 0.0f, -position.x});
    vertical_rotation_axis = camera.up;
}

Vector3 ModelRender::calculate_camera_position() {
    const auto& bounds = model->bounds;
    
    Vector3 center = {
        (bounds[0] + bounds[1]) / 2.0f,
        (bounds[2] + bounds[3]) / 2.0f,
        (bounds[4] + bounds[5]) / 2.0f
    };
    
    float rx = (bounds[1] - bounds[0]) / 2.0f;
    float ry = (bounds[3] - bounds[2]) / 2.0f;
    float rz = (bounds[5] - bounds[4]) / 2.0f;
    float radius = std::sqrt(rx*rx + ry*ry + rz*rz);
    
    float aspect_ratio_val = aspect_ratio.x / aspect_ratio.y;
    float fov_y_radians = camera_fovy * DEG2RAD;
    float fov_x_radians = 2.0f * std::atan(std::tan(fov_y_radians / 2.0f) * aspect_ratio_val);
    
    float limiting_fov = std::min(fov_y_radians, fov_x_radians);
    float distance = radius / std::sin(limiting_fov / 2.0f);
    distance *= (1.0f + camera_margin);
    
    float angle_x_rad = initial_view_angles.x * DEG2RAD;
    float angle_y_rad = initial_view_angles.y * DEG2RAD;
    float angle_z_rad = initial_view_angles.z * DEG2RAD;
    
    Vector3 offset = {
        distance * std::sin(angle_x_rad),
        distance * std::sin(angle_y_rad),
        distance * std::sin(angle_z_rad)
    };
    
    return Vector3Add(center, offset);
}

void ModelRender::rotate_horizontally(bool clockwise) {
    float speed = clockwise ? camera_speed : -camera_speed;
    
    camera.position = Vector3RotateByAxisAngle(camera.position,
         vertical_rotation_axis, speed);
    horizontal_rotation_axis = Vector3RotateByAxisAngle(horizontal_rotation_axis,
        vertical_rotation_axis, speed);
}

void ModelRender::rotate_vertically(bool clockwise) {
    float speed = clockwise ? camera_speed : -camera_speed;
    camera.position = Vector3RotateByAxisAngle(camera.position,
        horizontal_rotation_axis, speed);
}

void ModelRender::move_camera() {
    if (IsKeyPressed(KEY_SPACE)) {
        auto_rotate = !auto_rotate;
    }
    
    if (auto_rotate) {
        rotate_horizontally(true);
    } else {
        if (IsKeyDown(KEY_RIGHT)) {
            rotate_horizontally(false);
        } else if (IsKeyDown(KEY_LEFT)) {
            rotate_horizontally(true);
        }
    }
    
    if (IsKeyDown(KEY_UP)) {
        rotate_vertically(true);
    } else if (IsKeyDown(KEY_DOWN)) {
        rotate_vertically(false);
    }
}

void ModelRender::zoom() {
    float wheel_move = GetMouseWheelMove();
    if (wheel_move == 0.0f) return;
    
    Vector3 norm = Vector3Normalize(camera.position);
    Vector3 movement = Vector3Scale(norm, 2.0f);
    
    if (wheel_move < 0) {
        // Zoom out, away from the object
        camera.position = Vector3Add(camera.position, movement);
    } else if (wheel_move > 0) {
        // Zoom in, towards the object
        camera.position = Vector3Subtract(camera.position, movement);
    }
}

void ModelRender::draw_help_box() {
    int header_height = static_cast<int>(box.height * 0.25f);
    int body_height = static_cast<int>(box.height) - header_height;
    
    Rectangle header_rect = {box.x, box.y, box.width, static_cast<float>(header_height)};
    Rectangle body_rect = {box.x, box.y + header_height, box.width, static_cast<float>(body_height)};
    
    Color header_bg_color = {204, 204, 204, 255};
    Color header_text_color = {0, 0, 0, 255};
    Color body_bg_color = {240, 240, 240, 136};
    Color body_text_color = {51, 51, 51, 255};
    Color key_text_color = {30, 30, 30, 255};
    Color border_color = {170, 170, 170, 255};
    
    // Draw header
    DrawRectangleRec(header_rect, header_bg_color);
    DrawText(
        "Object camera controls:",
        static_cast<int>(header_rect.x + 10),
        static_cast<int>(header_rect.y + header_height / 4),
        text_fontsize,
        header_text_color
    );
    
    // Draw body
    DrawRectangleRec(body_rect, body_bg_color);
    DrawRectangleLinesEx(box, 2.0f * height_scale, border_color);
    
    // Body content
    int top_padding = 15 * height_scale;
    int side_padding = 10 * width_scale;
    int body_fontsize = std::max(1, text_fontsize - 1);
    int line_spacing = static_cast<int>(body_fontsize * 1.5f);
    
    struct HelpLine {
        const char* key_part;
        const char* description_part;
    };
    
    HelpLine body_lines[] = {
        {"[Esc]", "to close the program"},
        {"[Mouse wheel]", "to zoom in-out"},
        {"[Right | Left | Up | Down]", "to rotate the object"},
        {"[Space]", "to toggle auto-rotate"},
    };
    
    for (size_t i = 0; i < sizeof(body_lines) / sizeof(body_lines[0]); ++i) {
        int cursor_x = static_cast<int>(body_rect.x + side_padding);
        int cursor_y = static_cast<int>(body_rect.y + top_padding + i * line_spacing);
        
        DrawText(body_lines[i].key_part, cursor_x, cursor_y, body_fontsize, key_text_color);
        cursor_x += MeasureText((std::string(body_lines[i].key_part) + " ").c_str(), body_fontsize);
        DrawText(body_lines[i].description_part, cursor_x, cursor_y, body_fontsize, body_text_color);
    }
}

void ModelRender::initialize() {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(0, 0, model->path.filename().string().c_str());
    SetTargetFPS(60);
    
    int monitor = GetCurrentMonitor();
    int screen_height = GetMonitorHeight(monitor);
    int screen_width = GetMonitorWidth(monitor);
    SetWindowSize(screen_width, screen_height);
    
    width_scale = screen_width / base_width;
    height_scale = screen_height / base_height;
    
    // Scale the box and font size
    box = {
        box.x * width_scale,
        box.y * height_scale,
        box.width * width_scale,
        box.height * height_scale
    };
    text_fontsize = text_fontsize * width_scale;
}

void ModelRender::draw_model(void) {
    for (const auto& cube_pos : this->model->cubes) {
        Vector3 center = {cube_pos.x, cube_pos.z, cube_pos.y};
        DrawCube(center, this->model->cube_dimensions.x, this->model->cube_dimensions.y,
                  this->model->cube_dimensions.z, WHITE);
        DrawCubeWires(center, this->model->cube_dimensions.x, this->model->cube_dimensions.y,
                       this->model->cube_dimensions.z, BLACK);
    }
}

void ModelRender::render_loop() {
    while (!WindowShouldClose()) {
        move_camera();
        zoom();
        
        BeginDrawing();
        ClearBackground({10, 10, 10, 255});
        
        BeginMode3D(camera);
        draw_model();
        EndMode3D();
        
        draw_help_box();
        EndDrawing();
    }
    
    CloseWindow();
}

