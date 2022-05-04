module;
#include <glm/glm.hpp>
#include <array>
#include <string>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
export module Renderer;

import Texture;
import Buffer;
import VertexArray;
import Shader;

export struct Vertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
};

export struct RendererStorage
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

	static constexpr uint32_t MaxTextureSlots = 32;
	std::array<Texture*, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 0;

	glm::vec4 unitQuad[4]
	{
		glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f},
		glm::vec4{0.5f, -0.5f, 0.0f, 1.0f},
		glm::vec4{0.5f, 0.5f, 0.0f, 1.0f},
		glm::vec4{-0.5f, 0.5f, 0.0f, 1.0f}
	};
};

export class Renderer
{
public:
	static Renderer& get()
	{
		static Renderer renderer;
		return renderer;
	}

	void init()
	{
		m_data.QuadVertexArray = new VertexArray;

		m_data.QuadVertexBuffer = new VertexBuffer(m_data.MaxVertices * sizeof(Vertex));
		BufferElement elem1(ShaderDataType::Float3, "a_Position");
		BufferElement elem2(ShaderDataType::Float4, "a_Color");
		BufferElement elem3(ShaderDataType::Float2, "a_TexCoord");
		BufferElement elem4(ShaderDataType::Float, "a_TexIndex");
		BufferLayout layout({elem1, elem2, elem3, elem4});

		m_data.QuadVertexBuffer->setLayout(layout);

		m_data.QuadVertexArray->addVertexBuffer(m_data.QuadVertexBuffer);

		m_data.QuadVertexBufferBase = new Vertex[RendererStorage::MaxVertices];
		m_data.QuadVertexBufferPtr = m_data.QuadVertexBufferBase;

		uint32_t* quadIndices = new uint32_t[RendererStorage::MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < m_data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		IndexBuffer* quadIB = new IndexBuffer(quadIndices, RendererStorage::MaxIndices);
		m_data.QuadVertexArray->setIndexBuffer(quadIB);
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

			m_data.TextureShader = new Shader("Texture", vertexSrc, fragmentSrc);
			m_data.TextureShader->bind();

			int32_t samplers[RendererStorage::MaxTextureSlots];
			for (uint32_t i = 0; i < RendererStorage::MaxTextureSlots; i++)
				samplers[i] = i;

			m_data.TextureShader->uploadUniformIntArray("u_Textures", samplers, RendererStorage::MaxTextureSlots);

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

			m_data.FlatColorShader = new Shader("Flat Color", vertexSrc, fragmentSrc);
		}

	}

	void shutdown()
	{
	}

	void setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void beginTextures(const glm::mat4& viewProjection)
	{
		m_data.QuadVertexArray->bind();
		m_data.QuadIndexCount = 0;
		m_data.QuadVertexBufferPtr = m_data.QuadVertexBufferBase;
		m_data.TextureShader->bind();
		m_data.TextureShader->uploadUniformMat4("u_ViewProjection", viewProjection);
	}

	void beginFlatColor(const glm::mat4& viewProjection)
	{
		m_data.QuadVertexArray->bind();
		m_data.QuadIndexCount = 0;
		m_data.QuadVertexBufferPtr = m_data.QuadVertexBufferBase;
		m_data.FlatColorShader->bind();
		m_data.FlatColorShader->uploadUniformMat4("u_ViewProjection", viewProjection);
	}

	void endTextures()
	{
		for (uint32_t i = 0; i < m_data.TextureSlotIndex; i++)
			m_data.TextureSlots[i]->bind(i);

		uint32_t dataSize = (uint8_t*)m_data.QuadVertexBufferPtr - (uint8_t*)m_data.QuadVertexBufferBase;
		m_data.QuadVertexBuffer->setData(m_data.QuadVertexBufferBase, dataSize);
		glDrawElements(GL_TRIANGLES, m_data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);
	}

	void endFlatColor()
	{
		uint32_t dataSize = (uint8_t*)m_data.QuadVertexBufferPtr - (uint8_t*)m_data.QuadVertexBufferBase;
		m_data.QuadVertexBuffer->setData(m_data.QuadVertexBufferBase, dataSize);
		glDrawElements(GL_TRIANGLES, m_data.QuadIndexCount, GL_UNSIGNED_INT, nullptr);
	}

	void pushQuad(const glm::mat4& transform, Texture* texture, const glm::vec4& color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, bool includeTextureSize = true)
	{
		if (m_data.QuadIndexCount >= RendererStorage::MaxIndices)
			nextBatch();

		float textureIndex = -1.0f;
		for (uint32_t i = 0; i < m_data.TextureSlotIndex; i++)
		{
			if (m_data.TextureSlots[i] == texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			textureIndex = (float)m_data.TextureSlotIndex;
			m_data.TextureSlots[m_data.TextureSlotIndex] = texture;
			m_data.TextureSlotIndex++;
		}

		auto tf = includeTextureSize ? (transform * glm::scale(glm::mat4(1), glm::vec3{ texture->getWidth(), texture->getHeight(), 1 })) : transform;

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[0] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 0.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[1] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 0.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[2] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 1.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[3] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 1.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadIndexCount += 6;
	}

	void pushQuad(const glm::mat4& transform, SubTexture2D* subTexture, const glm::vec4& color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, bool includeTextureSize = true)
	{
		if (m_data.QuadIndexCount >= RendererStorage::MaxIndices)
			nextBatch();

		auto& texture = subTexture->getTexture();

		float textureIndex = -1.0f;
		for (uint32_t i = 0; i < m_data.TextureSlotIndex; i++)
		{
			if (*m_data.TextureSlots[i] == texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			textureIndex = (float)m_data.TextureSlotIndex;
			m_data.TextureSlots[m_data.TextureSlotIndex] = &texture;
			m_data.TextureSlotIndex++;
		}

		glm::vec2 spriteSize = subTexture->getSize();
		auto tf = includeTextureSize ? (transform * glm::scale(glm::mat4(1), glm::vec3{ spriteSize.x, spriteSize.y, 1 })) : transform;
		const glm::vec2* texCoords = subTexture->getTexCoords();

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[0] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = texCoords[0];
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[1] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = texCoords[1];
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[2] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = texCoords[2];
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = { tf * m_data.unitQuad[3] };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = texCoords[3];
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadIndexCount += 6;
	}

	void pushQuad(const glm::vec2& min, const glm::vec2& max, Texture* texture, const glm::vec4& color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
	{
		if (m_data.QuadIndexCount >= RendererStorage::MaxIndices)
			nextBatch();

		float textureIndex = -1.0f;
		for (uint32_t i = 0; i < m_data.TextureSlotIndex; i++)
		{
			if (m_data.TextureSlots[i] == texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			textureIndex = (float)m_data.TextureSlotIndex;
			m_data.TextureSlots[m_data.TextureSlotIndex] = texture;
			m_data.TextureSlotIndex++;
		}

		m_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, min.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 0.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, min.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 0.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, max.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 1.0f, 1.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, max.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr->TexCoord = glm::vec2{ 0.0f, 1.0f };
		m_data.QuadVertexBufferPtr->TexIndex = textureIndex;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadIndexCount += 6;
	}

	void pushQuad(const glm::vec2& min, const glm::vec2& max, const glm::vec4& color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f })
	{
		if (m_data.QuadIndexCount >= RendererStorage::MaxIndices)
			nextBatch();


		m_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, min.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, min.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = glm::vec3{ max.x, max.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadVertexBufferPtr->Position = glm::vec3{ min.x, max.y, 0 };
		m_data.QuadVertexBufferPtr->Color = color;
		m_data.QuadVertexBufferPtr++;

		m_data.QuadIndexCount += 6;
	}

private:
	RendererStorage m_data;

	void nextBatch()
	{
		endTextures();
		m_data.QuadIndexCount = 0;
		m_data.QuadVertexBufferPtr = m_data.QuadVertexBufferBase;
		m_data.TextureSlotIndex = 0;
	}
};
