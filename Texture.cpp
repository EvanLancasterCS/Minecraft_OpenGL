#include "Texture.h"

Texture::Texture() {
	m_TextureID = -1;
}

Texture::Texture(std::vector<const char*> faces)
{
	loadCubeTexture(faces);
}

Texture::Texture(const char* fileName, bool normal)
{
	if (!normal)
		loadTexture(fileName);
	else
		loadNormalTexture(fileName);

	initializeTexture();
}



bool Texture::loadTexture(const char* texFile) {
	m_TextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (!m_TextureID) {
		std::cout << "Failed: Could not open texture file! name: " << texFile << "\n";
		return false;
	}
	return true;
}

bool Texture::loadNormalTexture(const char* texFile)
{
	m_TextureID = SOIL_load_OGL_texture(texFile, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA);
	if (!m_TextureID) {
		std::cout << "Failed: Could not open texture file! name: " << texFile << "\n";
		return false;
	}
	return true;
}

bool Texture::loadCubeTexture(std::vector<const char*> faces)
{
	int width, height, nrChannels;
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* image = SOIL_load_image(faces[i], &width, &height, &nrChannels, 0);
		if (image)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
			std::cout << "Failed to load image: " << faces[i] << "\n";
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}

bool Texture::initializeTexture()
{
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return true;
}



