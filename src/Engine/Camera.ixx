module;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
export module Camera;

class Camera
{
public:
	void setOrtho(float left, float right, float bottom, float top)
	{
		m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	void setPerspective(float fov, float aspect, float nearPlane, float farPlane)
	{
		m_projectionMatrix = glm::perspective(fov, aspect, nearPlane, farPlane);
	}

	glm::vec2 screenToWorldPoint(const glm::vec2& screenPoint, const glm::mat4& viewProjectionMat)
	{
		glm::vec2 mousePosClip = (2.0f * screenPoint) - 1.0f;
		mousePosClip *= -1.0f;
		return glm::vec2{ glm::inverse(viewProjectionMat) * glm::vec4{ mousePosClip, 0, 1 } };
	}

	const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }

private:
	glm::mat4 m_projectionMatrix;
};

class OrthoCamController
{
public:
	OrthoCamController() = default;

	void onUpdate()
	{
		/*
		if (Input::isKeyPressed(Key::A))
			m_position.x -= m_translationSpeed * Time::Delta;
		else if (Input::isKeyPressed(Key::D))
			m_position.x += m_translationSpeed * Time::Delta;

		if (Input::isKeyPressed(Key::W))
			m_position.y += m_translationSpeed * Time::Delta;
		else if (Input::isKeyPressed(Key::S))
			m_position.y -= m_translationSpeed * Time::Delta;
		*/

		m_translationSpeed = m_zoomLevel * 1000;
	}

	void setDimensions(float width, float aspectRatio)
	{
		m_inverseAspect = 1 / aspectRatio;
		m_width = width;
		m_camera.setOrtho(width * 0.5f * m_zoomLevel, -width * 0.5f * m_zoomLevel, -m_inverseAspect * width * 0.5f * m_zoomLevel, m_inverseAspect * width * 0.5f * m_zoomLevel);
	}

	/*
	bool OrthoCamController::onMouseScrolled(MouseScrolledEvent& e)
	{
		m_zoomLevel += e.GetYOffset() * m_scrollSpeed;
		m_zoomLevel = std::max(m_zoomLevel, 0.1f);
		return false;
	}
	*/

	float getWidth() { return m_width; }
	float getInverseAspect() { return m_inverseAspect; }
	float getZoomLevel() { return m_zoomLevel; }
	glm::mat4 getViewProjection() { return getProjection() * getView(); }
	glm::mat4 getView() { return glm::inverse(glm::translate(glm::mat4(1), m_position)); }
	glm::mat4 getProjection() { return m_camera.getProjectionMatrix(); }
	Camera& getCamera() { return m_camera; }

private:
	Camera m_camera;
	float m_width = 0;
	float m_inverseAspect = 0;
	float m_zoomLevel = 1;
	glm::vec3 m_position = glm::vec3{ 0.0f, 0.0f, 0.0f };
	float m_translationSpeed = 5.0f;
	float m_scrollSpeed = 0.5f;
};
