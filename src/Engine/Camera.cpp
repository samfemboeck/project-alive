#include "../pch.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

void Camera::setOrtho(float left, float right, float bottom, float top)
{
	projectionMatrix_ = glm::ortho(left, right, bottom, top);
}

void Camera::setPerspective(float fov, float aspect, float nearPlane, float farPlane)
{
	projectionMatrix_ = glm::perspective(fov, aspect, nearPlane, farPlane);
}

Vec2f Camera::screenToWorldPoint(const Vec2f& screenPoint, const glm::mat4& viewProjectionMat)
{
	Vec2f mousePosClip = (screenPoint * 2.0f) - 1.0f;
	mousePosClip.y *= -1.0f;
	return glm::vec2{ glm::inverse(viewProjectionMat) * glm::vec4{ mousePosClip.x, mousePosClip.y, 0, 1 } };
}