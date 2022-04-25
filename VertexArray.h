#pragma once

#include "Buffer.h"

class VertexArray
{
public:
	VertexArray();
	virtual ~VertexArray();

	virtual void bind() const;
	virtual void unbind() const;

	virtual void addVertexBuffer(VertexBuffer* vertexBuffer);
	virtual void setIndexBuffer(IndexBuffer* indexBuffer);

	virtual const std::vector<VertexBuffer*>& getVertexBuffers() const { return m_vertexBuffers; }
	virtual const IndexBuffer* getIndexBuffer() const { return m_indexBuffer; }

private:
	uint32_t m_rendererId;
	std::vector<VertexBuffer*> m_vertexBuffers;
	IndexBuffer* m_indexBuffer;
};
