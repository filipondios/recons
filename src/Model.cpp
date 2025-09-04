#include "Model.hpp"
#include "View.hpp"
#include <filesystem>
#include <iostream>


Model::Model(const std::filesystem::path &path, const int resolution) {
  if (!std::filesystem::exists(path))
    throw std::runtime_error("Not a valid path: " + path.string());

  this->space(resolution , resolution, resolution);
  this->space.setConstant(true);
  this->path = path;

  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_directory()) {
      try {
        const View view(entry.path());
        views.push_back(view);
        std::cout << view.to_string() << std::endl;

      } catch (const std::exception &e) {
        std::cerr << "Invalid view: " << entry.path() << ": "
           << e.what() << std::endl;
      }
    }
  }
}



void Model::model_refinement(void) {}
void Model::surface_generation(void) {}
void Model::draw_model(void) {}
