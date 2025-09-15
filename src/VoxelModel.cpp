#include <filesystem>
#include <iostream>
#include <algorithm>
#include "VoxelModel.hpp"

VoxelModel::VoxelModel(const std::filesystem::path &path, const int resolution,
    const bool print_info) : path(path), resolution(resolution),
    print_info(print_info) {

    if (!std::filesystem::exists(path))
        throw std::runtime_error("Not a valid path: " + path.string());
    bounds.fill(0.0f);

    // Load all views
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            try {
                View view(entry.path());
                views.push_back(view);
            } catch (const std::exception &e) {
                std::cerr << "Invalid view: " << entry.path() << ": "
                    << e.what() << std::endl;
            }
        }
    }

    if (views.empty()) {    
        throw std::runtime_error("No valid views found in: " + path.string());
    }

    std::cout << "[+] Starting initial reconstruction" << std::endl;
    this->print_model_info();
    this->calculate_bounds();
    this->initial_reconstruction();
    
    std::cout << "[+] Refining model" << std::endl;
    this->model_refinement();
    
    std::cout << "[+] Generating surface" << std::endl;
    this->surface_generation();

    if (print_info) {
        this->additional_info();
    }  
}

void VoxelModel::print_model_info() const {
    for (const auto& view : this->views) {
        std::cout << "[View " << view.name << "]" << std::endl
            << "Origin = [" << view.origin.x << ", " << view.origin.y << ", " << view.origin.z << "]" << std::endl
            << "Vx = [" << view.vx.x << ", " << view.vx.y << ", " << view.vx.z << "]" << std::endl
            << "Vy = [" << view.vy.x << ", " << view.vy.y << ", " << view.vy.z << "]" << std::endl
            << "Vz = [" << view.vz.x << ", " << view.vz.y << ", " << view.vz.z << "]" << std::endl
            << std::endl;
    }
}

void VoxelModel::calculate_bounds() {
    if (views.empty()) { 
        return; 
    }
        
    // Use the first view's limits (bounds)
    auto first_bounds = views[0].get_bounds();
    bounds[0] = first_bounds[0];  // x_min
    bounds[1] = first_bounds[2];  // x_max
    bounds[2] = first_bounds[1];  // y_min  
    bounds[3] = first_bounds[3];  // y_max
    bounds[4] = first_bounds[1];  // z_min
    bounds[5] = first_bounds[3];  // z_max
        
    for (size_t i = 1; i < views.size(); ++i) {
        auto view_bounds = views[i].get_bounds();
        bounds[0] = std::min(bounds[0], view_bounds[0]);  // x_min
        bounds[2] = std::min(bounds[2], view_bounds[1]);  // y_min
        bounds[1] = std::max(bounds[1], view_bounds[2]);  // x_max
        bounds[3] = std::max(bounds[3], view_bounds[3]);  // y_max
    }
        
    bounds[4] = bounds[2]; // z_min = y_min
    bounds[5] = bounds[3]; // z_max = y_max
}

void VoxelModel::initial_reconstruction() {
    // Initialize the voxel space to all voxels set (true)
    space = Eigen::Tensor<bool, SPACE_DIM>(resolution, resolution, resolution);
    space.setConstant(true);
}

void VoxelModel::model_refinement() {
    for (const auto& view : views) {
        std::cout << "[+] Using " << view.name << " to reconstruct." << std::endl;
        project_view_to_voxels(view);
    }
}

float VoxelModel::interpolate_bounds(float min_val, float max_val, int index) const {
    if (resolution <= 1) return min_val;
    return min_val + index * (max_val - min_val) / (resolution - 1);
}

void VoxelModel::project_view_to_voxels(const View& view) {
    View::Direction direction = view.get_direction();
        
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            Vector2 plane_point;
            Vector3 world_point;
                
            switch (direction) {
                case View::Direction::XY: {
                    // The view's plane is parallel to the XY space plane
                    const float wx = interpolate_bounds(bounds[0], bounds[1], i);
                    const float wy = interpolate_bounds(bounds[2], bounds[3], j);
                    world_point = {wx, wy, 0.0f};
                    plane_point = view.real_to_plane(world_point);
                            
                    if (!view.is_point_inside_contour(plane_point)) {
                        // Remove entire Z column
                        for (int k = 0; k < resolution; ++k) {
                            space(i, j, k) = false;
                        }
                    }
                    break;
                }

                case View::Direction::XZ: {
                    // The view's plane is parallel to the XZ space plane
                    const float wx = interpolate_bounds(bounds[0], bounds[1], i);
                    const float wz = interpolate_bounds(bounds[4], bounds[5], j);
                    world_point = {wx, 0.0f, wz};
                    plane_point = view.real_to_plane(world_point);
                            
                    if (!view.is_point_inside_contour(plane_point)) {
                        // Remove entire Y row
                        for (int k = 0; k < resolution; ++k) {
                            space(i, k, j) = false;
                        }
                    }
                    break;
                }

                case View::Direction::YZ: {
                    // The view's plane is parallel to the YZ space plane
                    const float wy = interpolate_bounds(bounds[2], bounds[3], i);
                    const float wz = interpolate_bounds(bounds[4], bounds[5], j);
                    world_point = {0.0f, wy, wz};
                    plane_point = view.real_to_plane(world_point);
                            
                    if (!view.is_point_inside_contour(plane_point)) {
                        // Remove entire X column
                        for (int k = 0; k < resolution; ++k) {
                            space(k, i, j) = false;
                        }
                    }
                    break;
                }
            }
        }
    }
}

void VoxelModel::surface_generation() {
    // Calculate cube dimensions
    float size_x = (bounds[1] - bounds[0]) / resolution;
    float size_y = (bounds[3] - bounds[2]) / resolution;
    float size_z = (bounds[5] - bounds[4]) / resolution;
    this->cube_dimensions = {size_x, size_y, size_z};   
    cubes.clear();

    for (int x = 0; x < resolution; ++x) {
        for (int y = 0; y < resolution; ++y) {
            for (int z = 0; z < resolution; ++z) {
                if (!space(x, y, z)) continue;
                
                const float cx = interpolate_bounds(bounds[0], bounds[1], x);
                const float cy = interpolate_bounds(bounds[2], bounds[3], y);
                const float cz = interpolate_bounds(bounds[4], bounds[5], z);
                cubes.push_back({cx, cy, cz});
            }
        }
    }
}

void VoxelModel::additional_info() const {
    std::cout << "[+] Model additional information:" << std::endl;
    std::cout << "[!] Model bounds: (" << bounds[0] << ", " << bounds[1] << ", " 
              << bounds[2] << ", " << bounds[3] << ", " 
              << bounds[4] << ", " << bounds[5] << ")" << std::endl;
    std::cout << "[!] Number of voxels: " << (resolution * resolution * resolution) << std::endl;
    std::cout << "[!] Number of active voxels: " << cubes.size() << std::endl;
}
