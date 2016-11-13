#pragma once

#include <glm/glm/glm.hpp>

class Observer {
	public:
		glm::dvec3 eye; //camera position
		glm::dvec3 cen; //reference point position
		glm::dvec3 up; //up vector direction (head of observer)

		Observer();
		Observer(const glm::dvec3 &_eye, const glm::dvec3 &_cen, const glm::dvec3 &_up);
		~Observer();

		void rotate_vert(const double alpha);
		void rotate_horz(const double alpha);

		void rotate_up(const double alpha);

		void zoom(const double scale);

		void forward(const double dist);
		void backward(const double dist);

		void follow_by_mouse(int lastx, int lasty, int posx, int posy);

		glm::vec3 get_eye_f() const;
		glm::vec3 get_cen_f() const;
		glm::vec3 get_up_f() const;
};
