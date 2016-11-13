#include "common.h"

bool LoadFile(const char *fileName, bool binary, uint8_t **buffer, uint32_t *size)
{
	assert(fileName);
	assert(buffer);
	assert(size);

	FILE     *input;
	uint32_t fileSize, readed;

	const char mode[] = {'r', binary ? 'b' : 't', '\0'};

	if ((input = fopen(fileName, mode)) == NULL)
		return false;

	fseek(input, 0, SEEK_END);
	fileSize = (uint32_t)ftell(input);
	rewind(input);

	if (fileSize == 0) {
		fclose(input);
		return false;
	}

	*buffer = new uint8_t[fileSize];
	assert(*buffer);

	readed = fread(*buffer, 1, fileSize, input);

	fclose(input);

	if (readed != fileSize)
	{
		delete[] *buffer;
		return false;
	}

	*size = fileSize;

	return true;
}
