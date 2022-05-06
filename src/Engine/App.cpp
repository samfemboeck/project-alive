#include "../pch.h"
#include "App.h"
#include "QuickMaths.h"
#include "Time.h"

void onWindowResize(GLFWwindow * m_window, int w, int h)
{
	glViewport(0, 0, w, h);
}

App::App()
{
	glfwSetErrorCallback([](int error_code, const char* desc) { LOG("GLFW error: {}", desc); });

	if (!glfwInit())
	{
		LOG("Failed to init GLFW!");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window_data.App = this;
	m_window_data.Width = 1600;
	m_window_data.Height = 800;

	m_window = glfwCreateWindow(m_window_data.Width, m_window_data.Height, "Project Alive", 0, nullptr);
	if (m_window == NULL)
	{
		LOG("Failed to create GLFW window!");
		glfwTerminate();
		return;
	}

	glfwSetWindowUserPointer(m_window, &m_window_data);

	glfwMakeContextCurrent(m_window);
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int w, int h)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->Width = w;
		data->Height = h;
		glViewport(0, 0, w, h);
	});

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG("Failed to initialize GLAD!");
		return;
	}

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x_pos, double y_pos)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->MousePos = Vec2(x_pos, y_pos);
	});

	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x_offset, double y_offset)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->App->on_mouse_scrolled(x_offset, y_offset);
	});

	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				data->App->on_mouse_pressed(button);
				break;
			}
			case GLFW_RELEASE:
			{
				data->App->on_mouse_released(button);
				break;
			}
		}
	});

	glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				data->App->on_key_pressed(key);
				break;
			}
			case GLFW_RELEASE:
			{
				data->App->on_key_released(key);
				break;
			}
		}
	});

	glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->App->on_window_close();
	});
}

void App::start()
{
	while (!glfwWindowShouldClose(m_window))
	{
		static clock_t start = clock();
		clock_t now = clock();
		Time::Delta = (now - start) / (float)CLOCKS_PER_SEC;
		start = now;
		Time::Elapsed += Time::Delta;
		update();
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void App::update()
{
}
