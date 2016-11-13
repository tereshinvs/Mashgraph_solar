#include <tuple>

#include "object.h"
#include "maths.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/rotate_vector.hpp>

Object::Object(const std::string &_name, glm::dvec3 &_cen, double _rad, double _log_rad,
	unsigned _data_count, VertexData *_pData, unsigned _indexes_count, unsigned *_pIndexes,
	GLuint _tex_id, GLenum _mode):
	name(_name), cen(_cen), rad(_rad), log_rad(_log_rad),
	pData(_pData), data_count(_data_count), pIndexes(_pIndexes), indexes_count(_indexes_count),
	tex_id(_tex_id), mode(_mode) {}

Object::Object(const std::string &_name, glm::dvec3 &_cen, double _rad, double _log_rad,
	std::tuple<unsigned, VertexData *, unsigned, unsigned *> data,
	GLuint _tex_id, GLenum _mode):
	name(_name), cen(_cen), rad(_rad), log_rad(_log_rad), tex_id(_tex_id), mode(_mode) {
	std::tie(data_count, pData, indexes_count, pIndexes) = data;
}

Object::~Object() {
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteTextures(1, &tex_id);
	if (pData) {
		delete[] pData;
		delete[] pIndexes;
	}
}

void Object::initGLBuffers(GLuint programId, const char *posName, const char *norName, const char *texName) {
	glDeleteBuffers(2, vbo);
	glDeleteVertexArrays(1, &vao);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, &vbo[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, data_count * sizeof(VertexData), pData, GL_STATIC_DRAW);
		
	glEnable(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes_count * sizeof(unsigned), pIndexes, GL_STATIC_DRAW);
	
	int	loc = glGetAttribLocation(programId, posName);
	if (loc > -1) {
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLuint *)0);
		glEnableVertexAttribArray(loc);
	}
	int loc2 = glGetAttribLocation(programId, norName);
	if (loc2 > -1) {
		glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLuint *)(0 + sizeof(float) * 3));
		glEnableVertexAttribArray(loc2);
	}
	int loc3 = glGetAttribLocation(programId, texName);
	if (loc3 > -1) {
		glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLuint *)(0 + sizeof(float) * 6));
		glEnableVertexAttribArray(loc3);
	}	
}

void Object::draw() {
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glDrawElements(mode, indexes_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Object::setTexId(GLuint new_tex_id) {
	tex_id = new_tex_id;
}

void Object::rotate_relatively_line(const glm::dvec3 &A, const glm::dvec3 &B, const double alpha) {
	cen = rotate_point_relatively_line(cen, A, B, alpha);
}

glm::dvec3 Object::get_vertex_pos(const unsigned num) const {
	return glm::dvec3(pData[num].pos[0] + cen[0], pData[num].pos[1] + cen[1], pData[num].pos[2] + cen[2]);
}

glm::dvec3 &Object::get_cen() const {
	return cen;
}

double Object::get_radius(bool log) const {
	return log ? log_rad : rad;
}

const std::string &Object::get_name() const {
	return name;
}

void Object::set_center(const glm::dvec3 &new_cen) {
	cen = new_cen;
}

void Object::set_spin_axe(const PlanetOrbitParams &params, double alpha) {
	glm::dvec3 _spin_axe = glm::rotate(params.OZ, alpha, params.V);
	spin_axe = glm::vec3(float(_spin_axe[0]), float(_spin_axe[1]), float(_spin_axe[2]));
	glm::dvec3 rot_axe = params.c_mat * params.V;
	for (unsigned i = 0; i < data_count; i++) {
		glm::dvec3 tmp = glm::dvec3(pData[i].pos[0], pData[i].pos[1], pData[i].pos[2]);
		tmp = glm::rotate(tmp, alpha, rot_axe);
		tmp = params.c_mat_inv * tmp;
		pData[i].pos = glm::vec3(float(tmp[0]), float(tmp[1]), float(tmp[2]));
		pData[i].nor = pData[i].pos;
	}
}

void Object::set_spin_time(double sec) {
	spin_time = sec;
}

void Object::self_rotate(double dt) {
	float alpha = float(360.0 / spin_time * dt);
	for (unsigned i = 0; i < data_count; i++) {
		pData[i].pos = glm::rotate(pData[i].pos, alpha, spin_axe);
		pData[i].nor = glm::rotate(pData[i].nor, alpha, spin_axe);
	}
}
