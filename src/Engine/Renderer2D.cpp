#include "../pch.h"
#include "Renderer2D.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
};

struct Renderer2DStorage
{
	static const uint32_t MaxQuads = 100000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;

	VertexArray* QuadVertexArray;
	VertexBuffer* QuadVertexBuffer;
	Shader* TextureShader;
	Shader* FlatColorShader;

	uint32_t QuadIndexCount = 0;
	Vertex* QuadVertexBufferBase = nullptr;
	Vertex* QuadVertexBufferPtr = nullptr;

	static const uint32_t MaxTextureSlots = 32;
	std::array<Texture2D*, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 0;

	glm::vec4 unitQuad[4]
	{
		glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f},
		glm::vec4{0.5f, -0.5f, 0.0f, 1.0f},
		glm::vec4{0.5f, 0.5f, 0.0f, 1.0f},
		glm::vec4{-0.5f, 0.5f, 0.0f, 1.0f}
	};

	Renderer2D::Statistics Stats;
};

static Renderer2DStorage s_data;

void Renderer2D::init()
{
	s_data.QuadVertexArray = new VertexArray;

	s_data.QuadVertexBuffer = new VertexBuffer(s_data.MaxVertices * sizeof(Vertex));
	s_data.QuadVertexBuffer->setLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Float, "a_TexIndex" }
		});

	s_data.QuadVertexArray->addVertexBuffer(s_data.QuadVertexBuffer);

	s_data.QuadVertexBufferBase = new Vertex[s_data.MaxVertices];
	s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

	uint32_t* quadIndices = new uint32_t[s_data.MaxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < s_data.MaxIndices; i += 6)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	IndexBuffer* quadIB = new IndexBuffer(quadIndices, s_data.MaxIndices);
	s_data.QuadVertexArray->setIndexBuffer(quadIB);
	delete[] quadIndices;

	{
		const char* vertexSrc = R"end(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			layout(location = 2) in vec2 a_TexCoord;
			layout(location = 3) in float a_TexIndex;

			uniform mat4 u_ViewProjection;

			out vec2 v_TexCoord;
			out vec4 v_Color;
			out float v_TexIndex;

			void main()
			{
				v_TexCoord = a_TexCoord;
				v_Color = a_Color;
				v_TexIndex = a_TexIndex;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
			)end";

		const char* fragmentSrc = R"end(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;
			in vec4 v_Color;
			in float v_TexIndex;

			uniform sampler2D u_Textures[32];

			void main()
			{
				color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * v_Color;
			}
			)end";

		s_data.TextureShader = new Shader("Texture", vertexSrc, fragmentSrc);
		s_data.TextureShader->bind();

		int32_t samplers[s_data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_data.TextureShader->uploadUniformIntArray("u_Textures", samplers, s_data.MaxTextureSlots);

		vertexSrc = R"end(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec4 v_Color;

			void main()
			{
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
			)end";

		fragmentSrc = R"end(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
			)end";

		s_data.FlatColorShader = new Shader("Flat Color", vertexSrc, fragmentSrc);
	}
}

void Renderer2D::shutdown()
{
}

void Renderer2D::setClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer2D::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer2D::beginTextures(const glm::mat4& viewProjection)
{
	s_data.QuadVertexArray->bind();
	s_data.QuadIndexCount = 0;
	s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;
	s_data.TextureShader->bind();
	s_data.TextureShader->uploadUniformMat4("u_ViewProjection", viewProjection);
}

void Renderer2D::beginFlatColor(const glm::mat4& viewProjection)
{
	s_data.QuadVertexArray->bind();
	s_data.QuadIndexCount = 0;
	s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;
	s_data.FlatColorShader->bind();
	s_data.FlatColorShader->uploadUniformMat4("u_ViewProjection", viewProjection);
}

void Renderer2D::endTextures()
{
	for (uint32_t i = 0; i < s_data.TextureSlotIndex; i++)
		s_data.TextureSlots[i]->bind(i);

	uint32_t dataSize = (uint8_t*)s_data.QuadVertexBufferPtr - (uint8_t*)s_data.QuadVertexBufferBase;
	s_data.QuadVertexBuffer->setData(s_data.QuadVertexBufferBase, dataSize);
	glDrawElements(GL_TRIANGLES, s_data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);

	s_data.Stats.DrawCalls++;
}

void Renderer2D::endFlatColor()
{
	uint32_t dataSize = (uint8_t*)s_data.QuadVertexBufferPtr - (uint8_t*)s_data.QuadVertexBufferBase;
	s_data.QuadVertexBuffer->setData(s_data.QuadVertexBufferBase, dataSize);
	glDrawElements(GL_TRIANGLES, s_data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);

	s_data.Stats.DrawCalls++;
}

void Renderer2D::pushQuad(const glm::mat4& transform, Texture2D* texture, const glm::vec4& color, bool includeTextureSize)
{
	if (s_data.QuadIndexCount >= Renderer2DStorage::MaxIndices)
		nextBatch();

	float textureIndex = -1.0f;
	for (uint32_t i = 0; i < s_data.TextureSlotIndex; i++)
	{
		if (s_data.TextureSlots[i] == texture)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == -1.0f)
	{
		textureIndex = (float)s_data.TextureSlotIndex;
		s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
		s_data.TextureSlotIndex++;
	}

	auto tf = includeTextureSize ? (transform * glm::scale(glm::mat4(1), glm::vec3{ texture->getWidth(), texture->getHeight(), 1 })) : transform;

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[0] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 0.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[1] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 0.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[2] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 1.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[3] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 1.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadIndexCount += 6;

	s_data.Stats.QuadCount++;
}

void Renderer2D::pushQuad(const glm::mat4& transform, SubTexture2D* subTexture, const glm::vec4& color, bool includeTextureSize)
{
	if (s_data.QuadIndexCount >= Renderer2DStorage::MaxIndices)
		nextBatch();

	auto& texture = subTexture->getTexture();

	float textureIndex = -1.0f;
	for (uint32_t i = 0; i < s_data.TextureSlotIndex; i++)
	{
		if (*s_data.TextureSlots[i] == texture)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == -1.0f)
	{
		textureIndex = (float)s_data.TextureSlotIndex;
		s_data.TextureSlots[s_data.TextureSlotIndex] = &texture;
		s_data.TextureSlotIndex++;
	}

	auto spriteSize = subTexture->getSize();
	auto tf = includeTextureSize ? (transform * glm::scale(glm::mat4(1), glm::vec3{ spriteSize.x, spriteSize.y, 1 })) : transform;
	auto texCoords = subTexture->getTexCoords();

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[0] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = texCoords[0];
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[1] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = texCoords[1];
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[2] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = texCoords[2];
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = { tf * s_data.unitQuad[3] };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = texCoords[3];
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadIndexCount += 6;

	s_data.Stats.QuadCount++;

}

void Renderer2D::pushQuad(const glm::vec2& min, const glm::vec2& max, Texture2D* texture, const glm::vec4& color)
{
	if (s_data.QuadIndexCount >= Renderer2DStorage::MaxIndices)
		nextBatch();

	float textureIndex = -1.0f;
	for (uint32_t i = 0; i < s_data.TextureSlotIndex; i++)
	{
		if (s_data.TextureSlots[i] == texture)
		{
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == -1.0f)
	{
		textureIndex = (float)s_data.TextureSlotIndex;
		s_data.TextureSlots[s_data.TextureSlotIndex] = texture;
		s_data.TextureSlotIndex++;
	}

	s_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, min.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 0.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, min.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 0.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, max.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 1.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, max.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 1.0f };
	s_data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadIndexCount += 6;

	s_data.Stats.QuadCount++;

}

void Renderer2D::pushQuad(const glm::vec2& min, const glm::vec2& max, const glm::vec4& color)
{
	if (s_data.QuadIndexCount >= Renderer2DStorage::MaxIndices)
		nextBatch();


	s_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, min.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, min.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, max.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, max.y, 0 };
	s_data.QuadVertexBufferPtr->Color = color;
	s_data.QuadVertexBufferPtr++;

	s_data.QuadIndexCount += 6;

	s_data.Stats.QuadCount++;
}

void Renderer2D::resetStats()
{
	memset(&s_data.Stats, 0, sizeof(s_data.Stats));
}

Renderer2D::Statistics Renderer2D::getStats()
{
	return s_data.Stats;
}

void Renderer2D::nextBatch()
{
	endTextures();
	s_data.QuadIndexCount = 0;
	s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;
	s_data.TextureSlotIndex = 0;
}