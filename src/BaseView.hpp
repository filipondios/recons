#pragma once

#include <clipper2/clipper.core.h>
#include <string>
#include <filesystem>
#include <raymath.h>
#include <clipper2/clipper.h>

struct BaseView {

	std::string name;
	Vector3 origin;
	Vector3 vx;
	Vector3 vy;
	Vector3 vz;
	Clipper2Lib::PathsD polygon;

	BaseView(const std::filesystem::path& path);
	Vector3 plane_to_real(const Vector2& point);
	Vector2 real_to_plane(const Vector3& point);
	std::string to_string(void) const;
};
