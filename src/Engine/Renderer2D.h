#pragma once

class Renderer2D
{
public:
	static void init();
	static void shutdown();

	static void setClearColor(const glm::vec4&);
	static void clear();

	static void beginTextures(const glm::mat4&);
	static void beginFlatColor(const glm::mat4&);
	static void endTextures();
	static void endFlatColor();

	static void pushQuad(const glm::mat4&, Texture2D*, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, bool = true);
	static void pushQuad(const glm::mat4&, SubTexture2D*, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, bool = true);
	static void pushQuad(const glm::vec2& min, const glm::vec2& max, Texture2D*, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	static void pushQuad(const glm::vec2& min, const glm::vec2& max, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t getTotalVertexCount() { return QuadCount * 4; }
		uint32_t getTotalIndexCount() { return QuadCount * 6; }
	};

	static void resetStats();
	static Statistics getStats();

private:
	static void nextBatch();
};
