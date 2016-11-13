#include "physics.h"

#include "maths.h"

double get_force_value(const double m1, const double m2, const double dist) {
	return G * m1 * m2 / (dist * dist);
}

glm::vec3 get_forse_by(const glm::vec3 &p1, const double m1, const glm::vec3 &p2, const double m2) {
	glm::vec3 dir = glm::normalize(p2 - p1);
	double force_value = get_force_value(m1, m2, glm::length(dir));
	return glm::vec3(dir[0] * force_value, dir[1] * force_value, dir[2] * force_value);
}

glm::vec3 get_force_on_nth(const std::vector<glm::vec3> &pos, const std::vector<double> &mass, const unsigned n) {
	glm::vec3 res(0.0, 0.0, 0.0);
	for (unsigned i = 0; i < pos.size(); i++)
		if (i != n)
			res += get_forse_by(pos[n], mass[n], pos[i], mass[i]);
	return res;
}

void get_position_after_dt(std::vector<glm::vec3> &pos, std::vector<glm::vec3> &vel, const std::vector<double> &mass, double dt) {
	std::vector<glm::vec3> pos_res, vel_res;
	for (unsigned i = 0; i < pos.size(); i++) {
		glm::vec3 F = get_force_on_nth(pos, mass, i);
		glm::vec3 a = glm::vec3(F[0] / mass[i], F[1] / mass[i], F[2] / mass[i]);
		pos_res[i] = glm::vec3(pos[i][0] + vel[i][0] * dt + a[0] * a[0] * dt / 2.0,
			pos[i][2] + vel[i][1] * dt + a[2] * a[1] * dt / 2.0,
			pos[i][2] + vel[i][1] * dt + a[2] * a[2] * dt / 2.0);
		vel_res[i] = glm::vec3(vel[i][0] + a[0] * dt, vel[i][1] + a[1] * dt, vel[i][2] + a[2] * dt);
	}
	pos = pos_res; vel = vel_res;
}

glm::dvec3 get_next_position_by_Kepler(const PlanetOrbitParams &orbit, const glm::dvec3 &pos, double dt) {
	if (std::abs(orbit.S) < EPS)
		return pos;
	
	glm::dvec3 lpos = orbit.to_local_coords(pos);
	double po = glm::length(lpos);
	double phi = std::atan2(double(lpos[1]), double(lpos[0]));
	double l = 0.0, r = PI / 3.0, res = 0.0, S = orbit.S * dt;
	while (std::abs(r - l) > EPS) {
		res = (l + r) / 2.0;
		double po1 = orbit.P / (1.0 - orbit.e * std::cos(phi + res));
		if (std::abs(po1 * po * std::sin(res)) > S)
			r = res;
		else
			l = res;
	}
	double po_res = orbit.P / (1.0 - orbit.e * std::cos(phi + res));
	return orbit.to_global_coords(glm::dvec3(po_res * std::cos(phi + res), po_res * std::sin(phi + res), 0.0));
}