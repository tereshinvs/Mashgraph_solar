#include "material.h"

Material::Material(const glm::vec4 &_ambient, const glm::vec4 &_diffuse,
	const glm::vec4 &_specular, const glm::vec4 &_emission, const float _shininess):
	ambient(_ambient), diffuse(_diffuse), specular(_specular), emission(_emission), shininess(_shininess) {}

Material::~Material() {}
