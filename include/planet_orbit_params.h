#pragma once

#include <string>

#include <glm/glm/glm.hpp>

struct PlanetOrbitParams {
	std::string orb_cen;
	glm::dvec3 &O;
	glm::dvec3 V, OX, OY, OZ;
	glm::dmat3x3 c_mat, c_mat_inv;
	double a, b, perigee, P, e, i;
	double S;

	PlanetOrbitParams(const std::string &_orb_cen,
		glm::dvec3 &_cen, const glm::dvec3 &pos, const glm::dvec3 &vel,
		double _perigee, double _a, double _e,
		double omega, double i, double w);
	~PlanetOrbitParams();

	glm::dvec3 to_local_coords(const glm::dvec3 &v) const;
	glm::dvec3 to_global_coords(const glm::dvec3 &v) const;

	glm::dvec2 to_ellipse(const glm::dvec2 &v) const;
};
