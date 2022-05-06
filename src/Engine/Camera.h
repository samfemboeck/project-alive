#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	static glm::vec2 screenToWorldPoint(const glm::vec2&, const glm::mat4&);

public:
	void setOrtho(float left, float right, float bottom, float top);
	void setPerspective(float fov, float aspect, float nearPlane, float farPlane);

	const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }

private:
	glm::mat4 m_projectionMatrix;
};
