#include <fstream>
#include <nlohmann/json.hpp>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include "BaseView.hpp"


void ensure_vector_format(const nlohmann::basic_json<> &item, const std::string &field) {
  if (!item.is_array() || item.size() != 3 ||
    !item[0].is_number() ||
    !item[1].is_number() ||
    !item[2].is_number()) {
    throw std::runtime_error("Invalid field '" + field + "'");
  }
}

BaseView::BaseView(const std::filesystem::path &path) {
  const auto camera_path = path / std::filesystem::path("camera.json");
  const auto plane_path = path / std::filesystem::path("plane.bmp");

  if (!std::filesystem::exists(camera_path) ||
      !std::filesystem::exists(plane_path)) {
    throw std::runtime_error("Missing required files");    
  }

  std::ifstream camera_stream(camera_path.string());
  nlohmann::json camera_data = nlohmann::json::parse(camera_stream);

  if (!camera_data.contains("name") || !camera_data.contains("origin") ||
      !camera_data.contains("vx") || !camera_data.contains("vy") ||
      !camera_data.contains("vz")) {
    throw std::runtime_error("Missing fields");    
  }

  ensure_vector_format(camera_data["origin"], "origin");
  ensure_vector_format(camera_data["vx"], "vx");
  ensure_vector_format(camera_data["vy"], "vy");
  ensure_vector_format(camera_data["vz"], "vz");

  if (!camera_data["name"].is_string()) {
    throw std::runtime_error("Invalid field 'name'");
  }

  // Initialize the view's camera position and orientation
  const std::vector<float> &origin = camera_data["origin"];  
  const std::vector<float> &vx = camera_data["vx"]; 
  const std::vector<float> &vy = camera_data["vy"]; 
  const std::vector<float> &vz = camera_data["vz"];

  this->name = camera_data["name"];   
  this->origin = Vector3 { origin[0], origin[1], origin[2] };
  this->vx = Vector3 { vx[0], vx[1], vx[2] };
  this->vy = Vector3 { vy[0], vy[1], vy[2] };
  this->vz = Vector3 { vz[0], vz[1], vz[2] };

  // Load the view's projection
  cv::Mat src, dst;
  src = cv::imread(plane_path.string(), cv::IMREAD_GRAYSCALE);
  cv::threshold(src, src, 254, 255, cv::THRESH_BINARY_INV);
  cv::Mat laplacian = (cv::Mat_<char>(3,3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
  cv::filter2D(src, dst, -1, laplacian);

  // Extract the polygonal line from the view's contour
}

Vector3 BaseView::plane_to_real(const Vector2 &point) {
  // Could've used Raylib's Vector3Add and Vector3Scale, but thats a lot of calls
  const float x{ this->origin.x + (this->vx.x * point.x) + (this->vz.x * point.y) };
  const float y{ this->origin.y + (this->vx.y * point.x) + (this->vz.y * point.y) };
  const float z{ this->origin.z + (this->vx.z * point.x) + (this->vz.z * point.y) };
  return Vector3{ x, y, z };
}

Vector2 BaseView::real_to_plane(const Vector3 &point) {
  const Eigen::Matrix<float, 3, 1> delta{
      point.x - this->origin.x,
      point.y - this->origin.y,
      point.z - this->origin.z,
  };

  const Eigen::Matrix<float, 3, 2> coeffs{
      {this->vx.x, this->vz.x},
      {this->vx.y, this->vz.y},
      {this->vx.z, this->vz.z},
  };

  const auto sol = coeffs.colPivHouseholderQr().solve(delta);
  return Vector2{sol(0), sol(1)};
}

std::string vector_to_string(const Vector3 &vector) {
  const auto x = std::to_string(vector.x);
  const auto y = std::to_string(vector.y);
  const auto z = std::to_string(vector.z);
  return "[" + x + "," + y + "," + z + "]";
}

std::string BaseView::to_string(void) const {
  return "[" + this->name + "]"
        + "\norigin = " + vector_to_string(this->origin)
        + "\nvx = " + vector_to_string(this->vx)
        + "\nvy = " + vector_to_string(this->vy)
        + "\nvz = " + vector_to_string(this->vz) + "\n";
}
