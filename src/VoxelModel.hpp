#pragma once
#include <array>
#include <vector>
#include <filesystem>
#include <unsupported/Eigen/CXX11/Tensor>
#include <raymath.h>
#include "View.hpp"

// space axes {x,y,z}
#define SPACE_DIM   3
// min{x,y,z}, max{x,y,z}
#define MNUM_BOUNDS 6


struct VoxelModel {

	std::vector<View> views;
	std::filesystem::path path;
	Eigen::Tensor<bool, SPACE_DIM> space;
	std::vector<Vector3> cubes;
	std::array<float, MNUM_BOUNDS> bounds;
	Vector3 cube_dimensions;
	int resolution;
	bool print_info;
	
	VoxelModel(const std::filesystem::path& path, int resolution, bool print_info);

private:
	void initial_reconstruction(void);
	void model_refinement(void);
	void surface_generation(void);
	void additional_info(void) const;
	void calculate_bounds(void);
	void print_model_info(void) const;
	void project_view_to_voxels(const View& view);
	float interpolate_bounds(float min_val, float max_val, int index) const;
};
