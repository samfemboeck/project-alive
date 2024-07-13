#include "../pch.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "QuickMaths.h"
#include "OrthoCamController.h"
#include "Time.h"

OrthoCamController& OrthoCamController::getInstance()
{
	static OrthoCamController controller;
	return controller;
}

void OrthoCamController::update()
{
	if (isKeyADown_)
		position_.x -= speedTranslation_ * Time::DeltaSeconds;
	else if (isKeyDDown_)
		position_.x += speedTranslation_ * Time::DeltaSeconds;

	if (isKeyWDown_)
		position_.y += speedTranslation_ * Time::DeltaSeconds;
	else if (isKeySDown_)
		position_.y -= speedTranslation_ * Time::DeltaSeconds;

	speedTranslation_ = zoomLevel_ * 1000;
}

void OrthoCamController::setDimensions(float width, float aspect)
{
	aspectRatio_ = aspect;
	width_ = width;
	camera_.setOrtho(-width * 0.5f * zoomLevel_, width * 0.5f * zoomLevel_, -aspect * width * 0.5f * zoomLevel_, aspect * width * 0.5f * zoomLevel_);
}

void OrthoCamController::onKeyPressed(int key)
{
	switch (key)
	{
		case GLFW_KEY_W:
			isKeyWDown_ = true;
			break;
		case GLFW_KEY_A:
			isKeyADown_ = true;
			break;
		case GLFW_KEY_S:
			isKeySDown_ = true;
			break;
		case GLFW_KEY_D:
			isKeyDDown_ = true;
			break;
		default:
			break;
	}
}

void OrthoCamController::onKeyReleased(int key)
{
	switch (key)
	{
	case GLFW_KEY_W:
		isKeyWDown_ = false;
		break;
	case GLFW_KEY_A:
		isKeyADown_ = false;
		break;
	case GLFW_KEY_S:
		isKeySDown_ = false;
		break;
	case GLFW_KEY_D:
		isKeyDDown_ = false;
		break;
	default:
		break;
	}
}

bool OrthoCamController::onMouseScrolled(double mouseOffsetY)
{
	zoomLevel_ -= mouseOffsetY * speedScroll_;
	zoomLevel_ = std::max(zoomLevel_, 0.1f);
	camera_.setOrtho(-width_ * 0.5f * zoomLevel_, width_ * 0.5f * zoomLevel_, -aspectRatio_ * width_ * 0.5f * zoomLevel_, aspectRatio_ * width_ * 0.5f * zoomLevel_);
	speedScroll_ = 1 / zoomLevel_ * zoomLevel_ * zoomLevel_ * 0.1f;
	return false;
}

float OrthoCamController::getZoomLevel()
{
	return zoomLevel_;
}

void OrthoCamController::setPosition(Vec2f pos)
{
	position_ = glm::vec3(pos.x, pos.y, 0.0f);
}

Vec2f OrthoCamController::getPosition()
{
	return { position_.x, position_.y };
}
