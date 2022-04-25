#pragma once

#include "Renderer/Camera.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bc 
{
	class OrthoCamController
	{
	public:
		OrthoCamController() = default;

		void onUpdate();
		void onEvent(Event&);
		void setDimensions(float width, float aspectRatio);
		float getWidth() { return m_width; }
		float getInverseAspect() { return m_inverseAspect; }
		float getZoomLevel() { return m_zoomLevel; }
		glm::mat4 getViewProjection() { return getProjection() * getView(); }
		glm::mat4 getView() { return glm::inverse(glm::translate(glm::mat4(1), m_position)); }
		glm::mat4 getProjection() { return m_camera.getProjectionMatrix(); }

		Camera& getCamera() { return m_camera; }

	private:
		bool onMouseScrolled(MouseScrolledEvent& e);

	private:
		Camera m_camera;

		float m_width = 0;
		float m_inverseAspect = 0;
		float m_zoomLevel = 1;
		glm::vec3 m_position = glm::vec3{ 0.0f, 0.0f, 0.0f };
		float m_translationSpeed = 5.0f;
		float m_scrollSpeed = 0.5f;
	};
}
