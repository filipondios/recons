#pragma once

#include <string>
#include <filesystem>
#include <raymath.h>
#include <opencv2/opencv.hpp>

struct BaseView {

	std::string name;
	Vector3 origin;
	Vector3 vx;
	Vector3 vy;
	Vector3 vz;
	cv::Mat projection;

	BaseView(const std::filesystem::path& path);
	Vector3 plane_to_real(const Vector2& point);
	Vector2 real_to_plane(const Vector3& point);
	std::string to_string(void) const;
};
