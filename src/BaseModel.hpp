#pragma once
#include <vector>
#include "BaseView.hpp"

struct BaseModel {
	
	std::vector<BaseView> views;
	std::filesystem::path path;

	BaseModel(const std::filesystem::path& path);
	virtual void initial_reconstruction(void);
	virtual void model_refinement(void);
	virtual void surface_generation(void);
	virtual void draw_model(void);
};
