#include "../pch.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

void Camera::setOrtho(float left, float right, float bottom, float top)
{
	m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void Camera::setPerspective(float fov, float aspect, float nearPlane, float farPlane)
{
	m_projectionMatrix = glm::perspective(fov, aspect, nearPlane, farPlane);
}

glm::vec2 Camera::screenToWorldPoint(const glm::vec2& screenPoint, const glm::mat4& viewProjectionMat)
{
	glm::vec2 mousePosClip = (2.0f * screenPoint) - 1.0f;
	mousePosClip *= -1.0f;
	return glm::vec2{ glm::inverse(viewProjectionMat) * glm::vec4{ mousePosClip, 0, 1 } };
}