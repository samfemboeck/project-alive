#include "bcpch.h"
#include "OrthoCamController.h"

namespace bc 
{
	void OrthoCamController::onUpdate()
	{
		if (Input::isKeyPressed(Key::A))
			m_position.x -= m_translationSpeed * Time::Delta;
		else if (Input::isKeyPressed(Key::D))
			m_position.x += m_translationSpeed * Time::Delta;

		if (Input::isKeyPressed(Key::W))
			m_position.y += m_translationSpeed * Time::Delta;
		else if (Input::isKeyPressed(Key::S))
			m_position.y -= m_translationSpeed * Time::Delta;

		m_translationSpeed = m_zoomLevel * 1000;
	}

	void OrthoCamController::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OrthoCamController::onMouseScrolled));
	}

	void OrthoCamController::setDimensions(float width, float aspectRatio)
	{
		m_inverseAspect = 1 / aspectRatio;
		m_width = width;
		m_camera.setOrtho(width * 0.5f * m_zoomLevel, -width * 0.5f * m_zoomLevel, -m_inverseAspect * width * 0.5f * m_zoomLevel, m_inverseAspect * width * 0.5f * m_zoomLevel);
	}

	bool OrthoCamController::onMouseScrolled(MouseScrolledEvent& e)
	{
		m_zoomLevel += e.GetYOffset() * m_scrollSpeed;
		m_zoomLevel = std::max(m_zoomLevel, 0.1f);
		return false;
	}
}