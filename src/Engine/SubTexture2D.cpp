#include "../pch.h"
#include "SubTexture2D.h"
#include "Texture.h"

SubTexture2D::SubTexture2D(Texture2D& texture, const glm::vec2& min, const glm::vec2& max, const glm::vec2& cellSize, const glm::vec2& spriteSize) :
	texture_(texture),
	spriteSize_(spriteSize),
	cellSize_(cellSize)
{
	texCoords_[0] = glm::vec2{ min.x, min.y };
	texCoords_[1] = glm::vec2{ max.x, min.y };
	texCoords_[2] = glm::vec2{ max.x, max.y };
	texCoords_[3] = glm::vec2{ min.x, max.y };
}

SubTexture2D* SubTexture2D::createFromCoords(Texture2D& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
{
	auto min = glm::vec2{ (coords.x * cellSize.x) / texture.getWidth(), (coords.y * cellSize.y) / texture.getHeight() };
	auto max = glm::vec2{ ((coords.x + spriteSize.x) * cellSize.x) / texture.getWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture.getHeight() };
	return new SubTexture2D(texture, min, max, cellSize, spriteSize);
}
