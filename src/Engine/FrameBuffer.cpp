#include "../pch.h"
#include "FrameBuffer.h"
#include <glad/glad.h>

FrameBuffer::FrameBuffer(float width, float height) :
	m_width(width),
	m_height(height)
{
	glGenTextures(1, &m_colorBufferRendererId);
	glBindTexture(GL_TEXTURE_2D, m_colorBufferRendererId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &m_rendererId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBufferRendererId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_rendererId);
}

void FrameBuffer::bind()
{
	if (m_isDirty)
	{
		loadFromPath();
		m_isDirty = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
	glViewport(0, 0, m_width, m_height);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::setSize(float width, float height)
{
	m_width = width;
	m_height = height;
	m_isDirty = true;
}

void FrameBuffer::loadFromPath()
{
	glDeleteFramebuffers(1, &m_rendererId);
	glDeleteTextures(1, &m_colorBufferRendererId);
	glGenTextures(1, &m_colorBufferRendererId);
	glBindTexture(GL_TEXTURE_2D, m_colorBufferRendererId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &m_rendererId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBufferRendererId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
