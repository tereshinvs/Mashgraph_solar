#include <cmath>

#include "maths.h"

#include <glm/glm/gtx/rotate_vector.hpp>

double sign(double x) {
	if (x > 0)
		return 1.0;
	if (x < 0)
		return -1.0;
	return 0.0;
}

std::tuple<double, double, double> get_sphere_coord(const glm::dvec3 &p) {
	return std::make_tuple(std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]),
		std::acos(p[2] / std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2])),
		std::atan2(p[1], p[0]));
}

glm::dvec3 rotate_point_relatively_line(const glm::dvec3 &p, const glm::dvec3 &A, const glm::dvec3 &B, const double alpha) {
	return glm::rotate(p - A, alpha, B - A) + A;
}

bool compare_vec3(const glm::dvec3 &v1, const glm::dvec3 &v2) {
	for (unsigned i = 0; i < 3; i++)
		if (v1[i] < v2[i])
			return true;
		else if (v1[i] > v2[i])
			return false;
	return false;
}

bool is_log_compatible(const glm::dvec3 &v) {
	return std::abs(v[0]) > 1.0 && std::abs(v[1]) > 1.0 && std::abs(v[2]) > 1.0;
}

glm::dvec3 vec_log(const glm::dvec3 &v) {
	return glm::dvec3(
		(std::log(std::abs(v[0])) + 1.0) * sign(v[0]),
		(std::log(std::abs(v[1])) + 1.0) * sign(v[1]),
		(std::log(std::abs(v[2])) + 1.0) * sign(v[2]));
}

double to_radians(double x) {
	return x * PI / 180.0;
}

double to_degrees(double x) {
	return x * 180.0 / PI;
}
