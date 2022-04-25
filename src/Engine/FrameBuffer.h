#pragma once

#include "Texture.h"

class FrameBuffer
{
public:
	FrameBuffer(float width, float height);
	~FrameBuffer();
	void bind();
	void unbind();
	void setSize(float width, float height);
	void loadFromPath();
	int getColorBufferRendererId() { return m_colorBufferRendererId; }

private:
	float m_width = 0, m_height = 0;
	bool m_isDirty = false;
	uint32_t m_rendererId;
	uint32_t m_colorBufferRendererId;
};
