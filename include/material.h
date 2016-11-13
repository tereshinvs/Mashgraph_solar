#pragma once

#include <glm/glm/glm.hpp>

struct Material {
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 emission;
	float shininess;

	Material(const glm::vec4 &, const glm::vec4 &, const glm::vec4 &, const glm::vec4 &, const float);
	~Material();
};
