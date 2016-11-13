#include <iostream>
#include <cctype>

#include "textures.h"

#include "common/common.h"

std::string to_lower(const std::string &s) {
  std::string res = s;
  for (unsigned i = 0; i < res.size(); i++)
    res[i] = std::tolower(res[i]);
  return res;
}

GLuint loadTextureFromFile(const std::string &filename) {
  if (filename.size() < 3)
    throw "Invalid file name";
  std::string ext = to_lower(filename.substr(filename.size() - 3, 3));
  if (ext == "bmp")
    return loadTextureFromBMPFile(filename.c_str());
  if (ext == "tga")
    return loadTextureFromTGAFile(filename.c_str());
}

GLuint loadTextureFromBMPFile(const char *filename) {
  uint8_t *buffer;
	uint32_t size;
	GLint format, internalFormat;
	GLuint texId;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (!LoadFile(filename, true, &buffer, &size)) {
    	std::cout << "Error while loading file" << std::endl;
    	return 0;
  	}

  	if (size < 54) {
  		std::cout << "Invalid BMP file" << std::endl;
  		return 0;
  	}

  	uint32_t width = *(uint32_t *)(buffer + 18);
  	uint32_t height = *(uint32_t *)(buffer + 22);

  	format = GL_BGR;
  	internalFormat = GL_RGB8;
	glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
      GL_UNSIGNED_BYTE, buffer + 54);

    delete[] buffer;
    return texId;  
}

struct TGAHeader
{
  uint8_t  idlength;
  uint8_t  colormap;
  uint8_t  datatype;
  uint8_t  colormapinfo[5];
  uint16_t xorigin;
  uint16_t yorigin;
  uint16_t width;
  uint16_t height;
  uint8_t  bitperpel;
  uint8_t  description;
};

GLuint loadTextureFromTGAFile(const char *filename) {
  TGAHeader *header;
  uint8_t *buffer;
  uint32_t size;
  GLint format, internalFormat;
  GLuint texId;

  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  if (!LoadFile(filename, true, &buffer, &size)) {
    std::cout << "Error while loading file" << std::endl;
    return 0;
  }
  if (size <= sizeof(TGAHeader)) {
    std::cout << "Too small file " << filename << std::endl;
    delete[] buffer;
    return 0;
  }

  header = (TGAHeader *)buffer;
  if (header->datatype != 2 || (header->bitperpel != 24 && header->bitperpel != 32)) {
    std::cout << "Wrong TGA format" << std::endl;
    delete[] buffer;
    return 0;
  }

  format = header->bitperpel == 24 ? GL_BGR : GL_BGRA;
  internalFormat = format == GL_BGR ? GL_RGB8 : GL_RGBA8;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header->width, header->height, 0, format,
    GL_UNSIGNED_BYTE, (const GLvoid *)(buffer + sizeof(TGAHeader) + header->idlength));

  delete[] buffer;
  return texId;  
}

GLuint get_flat_texture(const glm::ivec3 &colour) {
  GLubyte img_data[3] = { GLubyte(colour[0]), GLubyte(colour[1]), GLubyte(colour[2]) };
  GLuint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &img_data[0]);
  return texId;
}
