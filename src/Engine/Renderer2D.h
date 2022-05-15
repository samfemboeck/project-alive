#pragma once

#define BLACK glm::vec4(0, 0, 0, 1)
#define WHITE glm::vec4(1, 1, 1, 1)
#define RED glm::vec4(1, 0, 0, 1)
#define GREEN glm::vec4(0, 1, 0, 1)
#define BLUE glm::vec4(0, 0, 1, 1)

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	float texIndex;
};

class Renderer2D
{
public:
	static void init();
	static void shutdown();

	static void setClearColor(const glm::vec4&);
	static void clear();

	static void beginTextures(const glm::mat4& view, const glm::mat4& projection);
	static void beginFlatColor(const glm::mat4&);
	static void endTextures();
	static void endFlatColor();
	static void beginWater(const glm::mat4& view, const glm::mat4& projection);
	static void endWater();

	static void pushQuad(const glm::mat4&, Texture2D*, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, bool = true);
	static void pushQuad(const glm::mat4&, SubTexture2D*, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, bool = true);
	static void pushQuad(const glm::vec2& min, const glm::vec2& max, Texture2D*, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	static void pushQuad(const glm::vec2& min, const glm::vec2& max, const glm::vec4 & = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

private:
	static void nextBatch();
};

struct Renderer2DStorage
{
	static const uint32_t MaxQuads = 100000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 32;

	inline static glm::vec3 WaterColor = { 119.0f, 154.0f, 251.0f };
	inline static int TileLevel = 300;
	inline static glm::vec3 LightAttenuation = { 1.0f, 0.02f, 0.0001f };
	inline static glm::vec3 AmbientLight = { 0.0f, 0.0f, 0.05f };

	VertexArray* quadVertexArray;
	VertexBuffer* quadVertexBuffer;
	Shader* textureShader;
	Shader* flatColorShader;

	uint32_t quadIndexCount = 0;
	Vertex* quadVertexBufferBase = nullptr;
	Vertex* quadVertexBufferPtr = nullptr;

	std::array<Texture2D*, MaxTextureSlots> textureSlots;
	uint32_t textureSlotIndex = 0;

	Shader* waterShader;
	Texture2D* txWaterChannel0;
	Texture2D* txWaterChannel1;	

	glm::vec4 unitQuad[4]
	{
		glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f},
		glm::vec4{0.5f, -0.5f, 0.0f, 1.0f},
		glm::vec4{0.5f, 0.5f, 0.0f, 1.0f},
		glm::vec4{-0.5f, 0.5f, 0.0f, 1.0f}
	};
};

