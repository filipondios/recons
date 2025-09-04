#pragma once
#include <vector>
#include <unsupported/Eigen/CXX11/Tensor>
#include <raymath.h>
#include "View.hpp"

struct Model {

	std::vector<View> views;
	std::filesystem::path path;
	Eigen::Tensor<int, 3> space;
	std::vector<Vector3> cubes;
	Vector3 cube_dimensions;
	
	Model(const std::filesystem::path& path, const int resolution);
	void model_refinement(void);
	void surface_generation(void);
	void draw_model(void);
};
