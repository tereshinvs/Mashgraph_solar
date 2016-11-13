#include "observer.h"

#include "maths.h"

#include <glm/glm/gtx/rotate_vector.hpp>

Observer::Observer() {}

Observer::Observer(const glm::dvec3 &_eye, const glm::dvec3 &_cen, const glm::dvec3 &_up):
	eye(_eye), cen(_cen), up(glm::normalize(_up)) {}

Observer::~Observer() {}

void Observer::rotate_vert(const double alpha) {
	glm::dvec3 last_eye = eye;
	eye = cen + glm::rotate(eye - cen, alpha, glm::cross(eye - cen, up));
	up = glm::rotate(up, alpha, glm::cross(last_eye - cen, up));
}

void Observer::rotate_horz(const double alpha) {
	eye = cen + glm::rotate(eye - cen, alpha, up);
}

void Observer::rotate_up(const double alpha) {
	up = glm::rotate(up, alpha, eye - cen);
}

void Observer::zoom(const double scale) {
	eye = cen + (eye - cen) * scale;
}

void Observer::forward(const double dist) {
	glm::dvec3 dir = glm::normalize(cen - eye);
	cen += dir * dist;
	eye += dir * dist;
}

void Observer::backward(const double dist) {
	glm::dvec3 dir = glm::normalize(cen - eye);
	cen -= dir * dist;
	eye -= dir * dist;
}

void Observer::follow_by_mouse(int lastx, int lasty, int posx, int posy) {
	double horz_angle = (posx - lastx) / 10.0;
	cen = eye + glm::rotate(cen - eye, horz_angle, up);

	double vert_angle = (lasty - posy) / 10.0;
	glm::dvec3 tmp = glm::cross(up, cen - eye);
	cen = eye + glm::rotate(cen - eye, vert_angle, tmp);
	up = glm::rotate(up, vert_angle, tmp);
}

glm::vec3 Observer::get_eye_f() const {
	return glm::vec3(float(eye[0]), float(eye[1]), float(eye[2]));
}

glm::vec3 Observer::get_cen_f() const {
	return glm::vec3(float(cen[0]), float(cen[1]), float(cen[2]));

}

glm::vec3 Observer::get_up_f() const {
	return glm::vec3(float(up[0]), float(up[1]), float(up[2]));
}
