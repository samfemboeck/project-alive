module;
#include <string>
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <unordered_map>
export module Texture;

export class Texture
{
public:
	Texture(const std::string& path)
	{
		glGenTextures(1, &m_id);
		loadFromPath("Textures/" + path);
	}

	Texture()
	{
		glDeleteTextures(1, &m_id);
	}

	void bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void loadFromPath(const std::string& path)
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

		m_window_width = width;
		m_window_height = height;

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
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_window_width, m_window_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	uint32_t getWidth() const { return m_window_width; }
	uint32_t getHeight() const { return m_window_height; }
	uint32_t getId() const { return m_id; }
	std::string getFilePath() { return m_path; }
	bool operator==(const Texture& other) const { return m_id == other.m_id; }

private:
	std::string m_path;
	uint32_t m_window_width, m_window_height;
	uint32_t m_id;
};

export class SubTexture2D
{
public:

	SubTexture2D(Texture& texture, const glm::vec2& min, const glm::vec2& max, const glm::vec2& cellSize, const glm::vec2& spriteSize) :
		m_texture(texture),
		m_spriteSize(spriteSize),
		m_cellSize(cellSize)
	{
		m_texCoords[0] = glm::vec2{ min.x, min.y };
		m_texCoords[1] = glm::vec2{ max.x, min.y };
		m_texCoords[2] = glm::vec2{ max.x, max.y };
		m_texCoords[3] = glm::vec2{ min.x, max.y };
	}

	SubTexture2D* createFromCoords(Texture& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
	{
		auto min = glm::vec2{ (coords.x * cellSize.x) / texture.getWidth(), (coords.y * cellSize.y) / texture.getHeight() };
		auto max = glm::vec2{ ((coords.x + spriteSize.x) * cellSize.x) / texture.getWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture.getHeight() };
		return new SubTexture2D(texture, min, max, cellSize, spriteSize);
	}

	Texture& getTexture() const { return m_texture; }
	const glm::vec2* getTexCoords() { return m_texCoords; }
	const glm::vec2& getSize() { return m_spriteSize * m_cellSize; }

private:
	Texture& m_texture;
	glm::vec2 m_texCoords[4];
	glm::vec2 m_spriteSize;
	glm::vec2 m_cellSize;
};

export class TextureManager
{
public:
	static TextureManager& get()
	{
		static TextureManager manager;
		return manager;
	}

	void add(std::string filename)
	{
		Texture* texture = new Texture(filename);
		m_textures.insert({ filename, texture });
	}

	Texture* get(std::string filename)
	{
		auto it = m_textures.find(filename);

		if (it != m_textures.end())
			return it->second;
		else
			return nullptr;
	}

private:
	std::unordered_map<std::string, Texture*> m_textures;
};
