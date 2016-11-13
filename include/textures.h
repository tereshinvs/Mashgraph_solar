#pragma once

#include <GL/glew.h>
#include <glm/glm/glm.hpp>

GLuint loadTextureFromFile(const std::string &filename);
GLuint loadTextureFromTGAFile(const char *);
GLuint loadTextureFromBMPFile(const char *);

GLuint get_flat_texture(const glm::ivec3 &colour);
