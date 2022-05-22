#pragma once

class Texture2D;

class SubTexture2D
{
public:
	SubTexture2D(Texture2D& texture, const glm::vec2& min, const glm::vec2& max, const glm::vec2& cellSize, const glm::vec2& spriteSize);

	Texture2D& getTexture() const { return texture_; }
	const glm::vec2* getTexCoords() { return texCoords_; }
	const glm::vec2 getSize() { return spriteSize_ * cellSize_; }

	static SubTexture2D* createFromCoords(Texture2D& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize);
private:
	Texture2D& texture_;
	glm::vec2 texCoords_[4];
	glm::vec2 spriteSize_;
	glm::vec2 cellSize_;
};
