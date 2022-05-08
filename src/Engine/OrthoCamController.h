#pragma once

#include "Camera.h"

class OrthoCamController
{
public:
	OrthoCamController() = default;

	void update();
	void set_dimensions(float width, float aspect);
	void key_pressed(int key);
	void key_released(int key);
	bool mouse_scrolled(double mouse_offset_y);
	inline glm::mat4 get_view_projection() { return getProjection() * getView(); }
	inline glm::mat4 getView() { return glm::inverse(glm::translate(glm::mat4(1), m_position)); }
	inline glm::mat4 getProjection() { return m_camera.getProjectionMatrix(); }
	inline Camera& getCamera() { return m_camera; }

private:
	Camera m_camera;

	float m_aspect;
	float m_width = 0;
	float m_zoom_level = 1;
	glm::vec3 m_position = glm::vec3{ 0.0f, 0.0f, 0.0f };
	float m_translation_speed = 5.0f;
	float m_scroll_speed = 0.25f;
	bool m_key_w_down = false;
	bool m_key_a_down = false;
	bool m_key_s_down = false;
	bool m_key_d_down = false;
};
