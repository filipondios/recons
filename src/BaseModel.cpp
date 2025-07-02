#include "BaseModel.hpp"
#include "BaseView.hpp"
#include <filesystem>
#include <iostream>


BaseModel::BaseModel(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path))
    throw std::runtime_error("Not a valid path: " + path.string());

  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory()) {
      try {
        const BaseView view(entry.path());
        views.push_back(view);
        std::cout << view.to_string() << std::endl;

      } catch (const std::exception &e) {
        std::cerr << "Invalid view: "<< entry.path() << ":"
           << e.what() << std::endl;
      }
    }
  }
}

void BaseModel::initial_reconstruction(void) {}
void BaseModel::model_refinement(void) {}
void BaseModel::surface_generation(void) {}
void BaseModel::draw_model(void) {}
