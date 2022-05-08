#include "../pch.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "QuickMaths.h"
#include "OrthoCamController.h"
#include "Time.h"

void OrthoCamController::update()
{
	if (m_key_a_down)
		m_position.x -= m_translation_speed * Time::DeltaSeconds;
	else if (m_key_d_down)
		m_position.x += m_translation_speed * Time::DeltaSeconds;

	if (m_key_w_down)
		m_position.y += m_translation_speed * Time::DeltaSeconds;
	else if (m_key_s_down)
		m_position.y -= m_translation_speed * Time::DeltaSeconds;

	m_translation_speed = m_zoom_level * 1000;
}

void OrthoCamController::set_dimensions(float width, float aspect)
{
	m_aspect = aspect;
	m_width = width;
	m_camera.set_ortho(-width * 0.5f * m_zoom_level, width * 0.5f * m_zoom_level, -aspect * width * 0.5f * m_zoom_level, aspect * width * 0.5f * m_zoom_level);
}

void OrthoCamController::key_pressed(int key)
{
	switch (key)
	{
		case GLFW_KEY_W:
			m_key_w_down = true;
			break;
		case GLFW_KEY_A:
			m_key_a_down = true;
			break;
		case GLFW_KEY_S:
			m_key_s_down = true;
			break;
		case GLFW_KEY_D:
			m_key_d_down = true;
			break;
		default:
			break;
	}
}

void OrthoCamController::key_released(int key)
{
	switch (key)
	{
	case GLFW_KEY_W:
		m_key_w_down = false;
		break;
	case GLFW_KEY_A:
		m_key_a_down = false;
		break;
	case GLFW_KEY_S:
		m_key_s_down = false;
		break;
	case GLFW_KEY_D:
		m_key_d_down = false;
		break;
	default:
		break;
	}
}

bool OrthoCamController::mouse_scrolled(double mouse_offset_y)
{
	m_zoom_level -= mouse_offset_y * m_scroll_speed;
	m_zoom_level = std::max(m_zoom_level, 0.1f);
	m_camera.set_ortho(-m_width * 0.5f * m_zoom_level, m_width * 0.5f * m_zoom_level, -m_aspect * m_width * 0.5f * m_zoom_level, m_aspect * m_width * 0.5f * m_zoom_level);
	return false;
}