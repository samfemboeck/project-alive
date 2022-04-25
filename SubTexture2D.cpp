#include "bcpch.h"
#include "SubTexture2D.h"
#include "Texture.h"

SubTexture2D::SubTexture2D(Texture2D& texture, const glm::vec2& min, const glm::vec2& max, const glm::vec2& cellSize, const glm::vec2& spriteSize) :
	m_texture(texture),
	m_spriteSize(spriteSize),
	m_cellSize(cellSize)
{
	m_texCoords[0] = glm::vec2{ min.x, min.y };
	m_texCoords[1] = glm::vec2{ max.x, min.y };
	m_texCoords[2] = glm::vec2{ max.x, max.y };
	m_texCoords[3] = glm::vec2{ min.x, max.y };
}

SubTexture2D* SubTexture2D::createFromCoords(Texture2D& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
{
	auto min = glm::vec2{ (coords.x * cellSize.x) / texture.getWidth(), (coords.y * cellSize.y) / texture.getHeight() };
	auto max = glm::vec2{ ((coords.x + spriteSize.x) * cellSize.x) / texture.getWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture.getHeight() };
	return new SubTexture2D(texture, min, max, cellSize, spriteSize);
}
