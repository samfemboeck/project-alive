module;
#include <vector>
#include <glad/glad.h>
export module VertexArray;

import Buffer;

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:    return GL_FLOAT;
	case ShaderDataType::Float2:   return GL_FLOAT;
	case ShaderDataType::Float3:   return GL_FLOAT;
	case ShaderDataType::Float4:   return GL_FLOAT;
	case ShaderDataType::Mat3:     return GL_FLOAT;
	case ShaderDataType::Mat4:     return GL_FLOAT;
	case ShaderDataType::Int:      return GL_INT;
	case ShaderDataType::Int2:     return GL_INT;
	case ShaderDataType::Int3:     return GL_INT;
	case ShaderDataType::Int4:     return GL_INT;
	case ShaderDataType::Bool:     return GL_BOOL;
	}

	return 0;
}

export class VertexArray
{
public:
	VertexArray()
	{
		glGenVertexArrays(1, &m_rendererId);
	}

	void bind() const
	{
		glBindVertexArray(m_rendererId);
	}

	void unbind() const
	{
		glBindVertexArray(0);
	}

	void addVertexBuffer(VertexBuffer* vertexBuffer)
	{
		glBindVertexArray(m_rendererId);
		vertexBuffer->bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			index++;
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void setIndexBuffer(IndexBuffer* indexBuffer)
	{
		glBindVertexArray(m_rendererId);
		indexBuffer->bind();

		m_indexBuffer = indexBuffer;
	}

	virtual const std::vector<VertexBuffer*>& getVertexBuffers() const { return m_vertexBuffers; }
	virtual const IndexBuffer* getIndexBuffer() const { return m_indexBuffer; }

private:
	uint32_t m_rendererId;
	std::vector<VertexBuffer*> m_vertexBuffers;
	IndexBuffer* m_indexBuffer;
};
