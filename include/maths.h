#pragma once

#include <cmath>
#include <tuple>

#include <glm/glm/glm.hpp>

const double PI = std::acos(-1.0);
const double EPS = 1e-8;

double sign(double x);

std::tuple<double, double, double> get_sphere_coord(const glm::dvec3 &p);

glm::dvec3 rotate_point_relatively_line(const glm::dvec3 &p, const glm::dvec3 &A, const glm::dvec3 &B, const double alpha);

bool compare_vec3(const glm::dvec3 &v1, const glm::dvec3 &v2);

bool is_log_compatible(const glm::dvec3 &v);
glm::dvec3 vec_log(const glm::dvec3 &v);

double to_radians(double x);
double to_degrees(double x);
