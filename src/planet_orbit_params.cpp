#include "planet_orbit_params.h"

#include <cmath>

#include <glm/glm/gtx/rotate_vector.hpp>

#include "maths.h"

PlanetOrbitParams::PlanetOrbitParams(const std::string &_orb_cen,
	glm::dvec3 &_cen, const glm::dvec3 &pos, const glm::dvec3 &vel,
	double _perigee, double _a, double _e,
	double omega, double _i, double w):
	
	orb_cen(_orb_cen), O(_cen), a(_a), b(_a * std::sqrt(1 - _e * _e)), perigee(_perigee), P(b * b / a), e(_e), i(_i) {
	
	if (std::abs(P) < EPS)
		P = 0.0;
	
	V = glm::rotate(glm::dvec3(1.0, 0.0, 0.0), omega, glm::dvec3(0.0, 0.0, 1.0));
	OZ = glm::rotate(glm::dvec3(0.0, 0.0, 1.0), i, V);
	OX = glm::rotate(V, w, OZ);
	OY = glm::cross(OZ, OX);
	c_mat[0][0] = OX[0]; c_mat[0][1] = OY[0]; c_mat[0][2] = OZ[0];
	c_mat[1][0] = OX[1]; c_mat[1][1] = OY[1]; c_mat[1][2] = OZ[1];
	c_mat[2][0] = OX[2]; c_mat[2][1] = OY[2]; c_mat[2][2] = OZ[2];
	c_mat_inv = glm::transpose(c_mat);
	c_mat = glm::inverse(c_mat_inv);

	glm::dvec3 lpos = c_mat * (pos - O), lvel = c_mat * vel;
	glm::dvec2 npos = to_ellipse(glm::dvec2(double(lpos[0]) + double(lvel[0]), double(lpos[1]) + double(lvel[1])));
	S = std::abs(double(npos[0]) * double(lpos[1]) - double(npos[1]) * double(lpos[0]));
}

PlanetOrbitParams::~PlanetOrbitParams() {}

glm::dvec3 PlanetOrbitParams::to_local_coords(const glm::dvec3 &v) const {
	return c_mat * (v - O);
}

glm::dvec3 PlanetOrbitParams::to_global_coords(const glm::dvec3 &v) const {
	return c_mat_inv * v + O;
}

glm::dvec2 PlanetOrbitParams::to_ellipse(const glm::dvec2 &v) const {
	if (std::abs(glm::length(v)) < EPS)
		return v;
	double phi = std::atan2(double(v[1]), double(v[0]));
	double po = P / (1.0 - e * std::cos(phi));
	return glm::dvec2(po * std::cos(phi), po * std::sin(phi));
}
