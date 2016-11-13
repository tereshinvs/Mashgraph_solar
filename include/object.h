#pragma once

#include <tuple>
#include <algorithm>
#include <string>

#include <GL/glew.h>

#include "vertex_data.h"
#include "planet_orbit_params.h"

class Object {
	private:
		std::string name;

		VertexData *pData; //pointer to object's internal data
		unsigned data_count;

		unsigned *pIndexes; //pointer to indexes (list of vetrices) 
		unsigned indexes_count;

		unsigned vbo[2]; //VertexBufferObject one for MeshVertexData, another for Indexes
		unsigned vao;

		GLuint tex_id;

		glm::dvec3 &cen;
		glm::vec3 spin_axe;
		double rad, log_rad;
		double spin_time;

		GLenum mode;

	public:
		Object(const std::string &_name, glm::dvec3 &_cen, double _rad, double _log_rad,
			unsigned _data_count, VertexData *_pData, unsigned _indexes_count, unsigned *_pIndexes,
			GLuint _tex_id, GLenum _mode);
		Object(const std::string &_name, glm::dvec3 &_cen, double _rad, double _log_rad,
			std::tuple<unsigned, VertexData *, unsigned, unsigned *>,
			GLuint _tex_id, GLenum _mode);
		~Object();

		void initGLBuffers(unsigned programId, const char *posName, const char *norName, const char *texName);
		void draw();

		void setTexId(GLuint new_tex_id);
		void rotate_relatively_line(const glm::dvec3 &A, const glm::dvec3 &B, const double alpha);

		glm::dvec3 get_vertex_pos(const unsigned num) const;
		glm::dvec3 &get_cen() const;
		double get_radius(bool log) const;
		const std::string &get_name() const;

		void set_center(const glm::dvec3 &new_cen);
		void set_spin_axe(const PlanetOrbitParams &params, double alpha);
		void set_spin_time(double sec);

		void self_rotate(double dt);

		unsigned nord_pol;
		unsigned south_pol;
};
