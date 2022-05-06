#include "../pch.h"
#include "Buffer.h"
#include <glad/glad.h>
#include <cstdint>

VertexBuffer::VertexBuffer(uint32_t size)
{
	glGenBuffers(1, &m_rendererId);
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(const void* vertices, uint32_t size)
{
	glGenBuffers(1, &m_rendererId);
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_rendererId);
}

void VertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
}

void VertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setData(const void* data, uint32_t size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t count)
	: m_count(count)
{
	glGenBuffers(1, &m_rendererId);

	// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
	// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
	glBindBuffer(GL_ARRAY_BUFFER, m_rendererId);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_rendererId);
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId);
}

void IndexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}