#pragma once

#include <tuple>

#include "vertex_data.h"
#include "planet_orbit_params.h"

std::tuple<unsigned int, VertexDataDouble *, unsigned int, unsigned int *> get_tetrahedron(const double);

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_sphere(const glm::dvec3 &, const double);
std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_sphere_by_extension(const glm::dvec3 &, const double);
std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_sphere_by_coords(const glm::dvec3 &, const double);

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_orbit(const PlanetOrbitParams &orbit);

std::tuple<unsigned int, VertexData *, unsigned int, unsigned int *> get_rings(
	const glm::dvec3 &cent, const double radius1, const double radius2);
