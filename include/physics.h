#include <vector>

#include <glm/glm/glm.hpp>

#include "planet_orbit_params.h"

const double G = 6.6725 * 1e-11;

double get_force_value(const double m1, const double m2, const double dist);
glm::vec3 get_forse_by(const glm::vec3 &p1, const double m1, const glm::vec3 &p2, const double m2);

void get_position_after_dt(std::vector<glm::vec3> &pos, std::vector<glm::vec3> &vel, const std::vector<double> &mass, double dt);

glm::dvec3 get_next_position_by_Kepler(const PlanetOrbitParams &orbit, const glm::dvec3 &pos, double dt);
