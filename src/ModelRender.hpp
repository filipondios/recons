#include <raylib.h>
#include "VoxelModel.hpp"


struct ModelRender {

    const float camera_speed;
    const float camera_margin;
    const float camera_fovy;
    const float initial_view_angles[3];
    bool auto_rotate;

    const int base_width;
    const int base_height;
    const float aspect_ratio[2];
    int width_scale;
    int height_scale;
    int text_fontsize;
    int box[4];
    
    Camera3D camera;    
    const VoxelModel* model;
    Vector3 horizontal_rotation_axis;
    Vector3 vertical_rotation_axis;

    ModelRender(const VoxelModel* model);
    void initialize_render_context(void);
    void start_render_loop(void);

private:
    void setup_camera(void);
    Vector3 calculate_camera_position(void) const;
    void rotate_horizontally(bool clockwise);
    void rotate_vertically(bool clockwise);    
    void move_camera(void);
    void zoom(void);
    void draw_help_box(void) const;
    void draw_model(void) const;
};
