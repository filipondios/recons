#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <json/json.h>
#include <Eigen/Dense>
#include <nlohmann/json.hpp>
#include "View.hpp"
#include "raymath.h"


void ensure_vector_format(const nlohmann::basic_json<> &item, const std::string &field) {
  if (!item.is_array() || item.size() != 3 ||
    !item[0].is_number() ||
    !item[1].is_number() ||
    !item[2].is_number()) {
    throw std::runtime_error("Invalid field '" + field + "'");
  }
}

Clipper2Lib::Path64 get_contour_polygon(const cv::Mat& img) {
  const int height = img.rows;
  const int width = img.cols;
  
  // Find starting point
  std::pair<int, int> start{-1, -1};
  bool found = false;
  for (int z = 1; z < height - 1 && !found; ++z) {
    for (int x = 1; x < width - 1 && !found; ++x) {
      if (img.at<uchar>(z, x) == 0xff) {
        start = {x, z};
        found = true;
      }
    }
  }
  
  if (!found) { return {}; }
  
  // Direction vectors: right, down, left, up
  const std::vector<std::pair<int, int>> directions = {
    {1, 0}, {0, 1}, {-1, 0}, {0, -1}
  };

  // Initial, previous and current pixels
  int ix = start.first, iz = start.second;
  int px = ix, pz = iz;
  int cx = ix, cz = iz; 
  Clipper2Lib::Path64 points;
  
  do {
    // Check if current pixel is a vertex
    uchar horz = img.at<uchar>(cz, cx - 1) | img.at<uchar>(cz, cx + 1);
    uchar vert = img.at<uchar>(cz - 1, cx) | img.at<uchar>(cz + 1, cx);
    
    if (horz == 0xff && vert == 0xff) {
      // Vertex found (x, -z)
      points.push_back(Clipper2Lib::Point64{
        static_cast<int64_t>(cx),
        static_cast<int64_t>(-cz)
      });
    }
    
    // Find next pixel in contour
    bool next_found = false;
    for (const auto& dir : directions) {
      int nx = cx + dir.first;
      int nz = cz + dir.second;
      
      // Check bounds
      if (nx >= 0 && nx < width && nz >= 0 && nz < height) {
        if (img.at<uchar>(nz, nx) == 0xff && (nx != px || nz != pz)) {
          px = cx;
          pz = cz;
          cx = nx;
          cz = nz;
          next_found = true;
          break;
        }
      }
    }
    
    if (!next_found) {
      break;
    }
    
  } while (cx != ix || cz != iz);  
  return points;  
}

View::View(const std::filesystem::path &path) {
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

  // Extract the view's contour polygonal line
  // Then center the points for normalization
  auto vertices = get_contour_polygon(dst);

  if (!vertices.empty()) {
    double min_x = vertices[0].x, max_x = vertices[0].x;
    double min_y = vertices[0].y, max_y = vertices[0].y;

    for (const auto& vertex : vertices) {
      min_x = std::min(min_x, static_cast<double>(vertex.x));
      max_x = std::max(max_x, static_cast<double>(vertex.x));
      min_y = std::min(min_y, static_cast<double>(vertex.y));
      max_y = std::max(max_y, static_cast<double>(vertex.y));
    }

    const double center_x = (min_x + max_x) / 2.0;
    const double center_y = (min_y + max_y) / 2.0;

    Clipper2Lib::PathD path;
    for (const auto& vertex : vertices) {
      path.push_back(Clipper2Lib::PointD{
        static_cast<double>(vertex.x) - center_x,
        static_cast<double>(vertex.y) - center_y
      });
    }
    this->polygon.push_back(path);
  }
}

Vector3 View::plane_to_real(const Vector2 &point) const {
  // Could've used Raylib's Vector3Add and Vector3Scale, but thats a lot of calls
  const float x{ this->origin.x + (this->vx.x * point.x) + (this->vz.x * point.y) };
  const float y{ this->origin.y + (this->vx.y * point.x) + (this->vz.y * point.y) };
  const float z{ this->origin.z + (this->vx.z * point.x) + (this->vz.z * point.y) };
  return Vector3{ x, y, z };
}

Vector2 View::real_to_plane(const Vector3 &point) const {
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

bool View::is_point_inside_contour(const Vector2& point) const {
  if (polygon.empty() || polygon[0].empty()) {
    return false;
  }
    
  const auto& path = polygon[0];
  bool inside = false;
  size_t j = path.size() - 1;

  // Use ray-casting to chek if the point is inside the
  // view's contour polygon
  for (size_t i = 0; i < path.size(); i++) {
    const auto& pi = path[i];
    const auto& pj = path[j];
        
    if (((pi.y > point.y) != (pj.y > point.y)) &&
      (point.x < (pj.x - pi.x) * (point.y - pi.y) / (pj.y - pi.y) + pi.x)) {
      inside = !inside;
      break;
    }
    j = i;
  }
  return inside;
}

std::array<float, VNUM_BOUNDS> View::get_bounds(void) const {
  if (polygon.empty() || polygon[0].empty()) {
    return {0.0, 0.0, 0.0, 0.0};
  }

  const auto& path = polygon[0];
  float min_x = path[0].x, max_x = path[0].x;
  float min_y = path[0].y, max_y = path[0].y;
    
  for (const auto& point : path) {
    min_x = std::min(min_x, static_cast<float>(point.x));
    max_x = std::max(max_x, static_cast<float>(point.x));
    min_y = std::min(min_y, static_cast<float>(point.y));
    max_y = std::max(max_y, static_cast<float>(point.y));
  }
  return {min_x, min_y, max_x, max_y};
}

std::string vector_to_string(const Vector3 &vector) {
  const auto x = std::to_string(vector.x);
  const auto y = std::to_string(vector.y);
  const auto z = std::to_string(vector.z);
  return "[" + x + "," + y + "," + z + "]";
}

std::string View::to_string(void) const {
  return "[" + this->name + "]"
    + "\norigin = " + vector_to_string(this->origin)
    + "\nvx = " + vector_to_string(this->vx)
    + "\nvy = " + vector_to_string(this->vy)
    + "\nvz = " + vector_to_string(this->vz) + "\n";
}

View::Direction View::get_direction(void) const {
  // Return the axis of the space aligned with vy
  const Vector3 axisX = {1.0f, 0.0f, 0.0f};
  const Vector3 axisY = {0.0f, 1.0f, 0.0f};
  const Vector3 axisZ = {0.0f, 0.0f, 1.0f};

  // Dot products en valor absoluto
  const float dotX = std::fabs(Vector3DotProduct(this->vy, axisX));
  const float dotY = std::fabs(Vector3DotProduct(this->vy, axisY));
  const float dotZ = std::fabs(Vector3DotProduct(this->vy, axisZ));

  if (dotX >= dotY && dotX >= dotZ) {
    return View::Direction::YZ;
  } else if (dotY >= dotX && dotY >= dotZ) {
    return View::Direction::XZ;
  } else { return View::Direction::XY; }
}
