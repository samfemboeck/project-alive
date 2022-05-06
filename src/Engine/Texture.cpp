#include "../pch.h"
#include "Texture.h"
#include "stb_image.h"
#include <glad/glad.h>

Texture2D::Texture2D(const std::string& path)
{
	glGenTextures(1, &m_id);
	loadFromPath("Textures/" + path);
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &m_id);
}

void Texture2D::bind(uint32_t slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::loadFromPath(const std::string& path)
{
	m_path = path;

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);

	stbi_uc* data = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
		return;
	}

	m_width = width;
	m_height = height;

	GLenum internalFormat = 0, dataFormat = 0;
	if (channels == 4)
	{
		internalFormat = GL_RGBA8;
		dataFormat = GL_RGBA;
	}
	else if (channels == 3)
	{
		internalFormat = GL_RGB8;
		dataFormat = GL_RGB;
	}

	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}