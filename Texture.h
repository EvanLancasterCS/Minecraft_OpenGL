#pragma once
#include <vector>

#include "graphics_headers.h"
#include <SOIL2/SOIL2.h>


class Texture
{

public:
	Texture();
	Texture(std::vector<const char*> faces);
	Texture(const char* fileName, bool normal);
	Texture(const char* fileName);
	bool loadTexture(const char* texFile);
	bool loadNormalTexture(const char* texFile);
	bool loadCubeTexture(std::vector<const char*> faces);
	GLuint getTextureID() const { return m_TextureID; }

private:
	GLuint m_TextureID{};

	static bool initializeTexture();

};

