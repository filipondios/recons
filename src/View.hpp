#pragma once
#include <filesystem>
#include <array>
#include <string>
#include <clipper2/clipper.h>
#include <raymath.h>

#define VNUM_BOUNDS 4 // min{x,y}, max{x,y}

struct View {

	enum Direction {
		XZ = 0x0,
		XY = 0x1,
		YZ = 0x2,
	};

	std::string name;
	Vector3 origin;
	Vector3 vx;
	Vector3 vy;
	Vector3 vz;
	Clipper2Lib::PathsD polygon;

	View(const std::filesystem::path& path);
	Vector3 plane_to_real(const Vector2& point) const;
	Vector2 real_to_plane(const Vector3& point) const;
	std::string to_string(void) const;
	View::Direction get_direction(void) const;
	bool is_point_inside_contour(const Vector2& point) const;
	std::array<float, VNUM_BOUNDS> get_bounds(void) const;
};
