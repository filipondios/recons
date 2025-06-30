#include <toml++/toml.hpp>
#include <Eigen/Dense>
#include "BaseView.hpp"


Vector3 parse_vector3(const toml::node_view<const toml::node>& node,
	const std::string& tag) {
	if (!node) throw std::runtime_error("Missing field: " + tag);

	auto v = node.as_array();

	if (!v || v->size() != 3 || !v->get(0)->is_number()
		|| !v->get(1)->is_number()
		|| !v->get(2)->is_number())
		throw std::runtime_error("Invalid field: " + tag);

	return Vector3{ static_cast<float>(v->get(0)->value_or(0.)),
		static_cast<float>(v->get(1)->value_or(0.)),
		static_cast<float>(v->get(2)->value_or(0.)) };
}


BaseView::BaseView(const std::filesystem::path& path) {
	const auto camera_info = path / std::filesystem::path("camera.toml");
	const auto plane = (path / std::filesystem::path("plane.bmp"));
	const toml::table camera = toml::parse_file(camera_info.string());

	this->name = camera["name"].value_or(path.filename().string());
	this->origin = parse_vector3(camera["origin"], "origin");
	this->vx = parse_vector3(camera["vx"], "vx");
	this->vy = parse_vector3(camera["vy"], "vy");
	this->vz = parse_vector3(camera["vz"], "vz");
	this->projection = cv::imread(plane.string(), cv::IMREAD_GRAYSCALE);
}


Vector3 BaseView::plane_to_real(const Vector2& point) {
	// Could've used Raylib's Vector3Add and Vector3Scale, but thats a lot of calls 
	const float x{ this->origin.x + (this->vx.x * point.x) + (this->vz.x * point.y) };
	const float y{ this->origin.y + (this->vx.y * point.x) + (this->vz.y * point.y) };
	const float z{ this->origin.z + (this->vx.z * point.x) + (this->vz.z * point.y) };
	return Vector3{ x, y, z };
}


Vector2 BaseView::real_to_plane(const Vector3& point) {
	const Eigen::Matrix<float, 3, 1> delta {
		point.x - this->origin.x,
		point.y - this->origin.y,
		point.z - this->origin.z,
	};

	const Eigen::Matrix<float, 3, 2> coeffs {
		{ this->vx.x, this->vz.x },
		{ this->vx.y, this->vz.y },
		{ this->vx.z, this->vz.z },
	};

	const auto sol = coeffs.colPivHouseholderQr().solve(delta);
	return Vector2{ sol(0), sol(1) };
}