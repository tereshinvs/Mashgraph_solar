#pragma once

#include <glm/glm/glm.hpp>

class VertexData {
	public:
		glm::vec3 pos;
		glm::vec3 nor;
		glm::vec2 tex;
};

class VertexDataDouble {
	public:
		glm::dvec3 pos;
		glm::dvec3 nor;
		glm::dvec2 tex;
};
