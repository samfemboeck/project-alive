module;
#include <glad/glad.h>
#include <cstdint>
export module FrameBuffer;

export class FrameBuffer
{
public:
	FrameBuffer(float width, float height) :
		m_window_width(width),
		m_window_height(height)
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

	~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_rendererId);
	}

	void bind()
	{
		if (m_isDirty)
		{
			loadFromPath();
			m_isDirty = false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
		glViewport(0, 0, m_window_width, m_window_height);
	}

	void unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void setSize(float width, float height)
	{
		m_window_width = width;
		m_window_height = height;
		m_isDirty = true;
	}

	void loadFromPath()
	{
		glDeleteFramebuffers(1, &m_rendererId);
		glDeleteTextures(1, &m_colorBufferRendererId);
		glGenTextures(1, &m_colorBufferRendererId);
		glBindTexture(GL_TEXTURE_2D, m_colorBufferRendererId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_window_width, m_window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &m_rendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBufferRendererId, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int getColorBufferRendererId() { return m_colorBufferRendererId; }

private:
	float m_window_width = 0, m_window_height = 0;
	bool m_isDirty = false;
	uint32_t m_rendererId;
	uint32_t m_colorBufferRendererId;
};
