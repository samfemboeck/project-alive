#pragma once

class Texture2D;

class SubTexture2D
{
public:
	SubTexture2D(Texture2D& texture, const glm::vec2& min, const glm::vec2& max, const glm::vec2& cellSize, const glm::vec2& spriteSize);

	Texture2D& getTexture() const { return m_texture; }
	const glm::vec2* getTexCoords() { return m_texCoords; }
	const glm::vec2& getSize() { return m_spriteSize * m_cellSize; }

	static SubTexture2D* createFromCoords(Texture2D& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize);
private:
	Texture2D& m_texture;
	glm::vec2 m_texCoords[4];
	glm::vec2 m_spriteSize;
	glm::vec2 m_cellSize;
};
