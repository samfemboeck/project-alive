module;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
export module App;

void error_callback(int error_code, const char* desc)
{
	std::cout << desc << std::endl;
}

export class App
{
public:
	App()
	{
		glfwSetErrorCallback([](int error_code, const char* desc) { std::cout << desc << std::endl; });

		if (!glfwInit())
		{
			std::cout << "Failed to init glfw" << std::endl;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_window_width, m_window_height, "Project Alive", 0, nullptr);
		if (m_window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(m_window);
		glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
			//m_window_width = width;
			//m_window_height = height;
			glViewport(0, 0, width, height);
		});
		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {});

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
	}

	void start()
	{
		while (!glfwWindowShouldClose(m_window))
		{
			static clock_t start = clock();
			clock_t now = clock();
			float deltaTime = (now - start) / (float)CLOCKS_PER_SEC;
			start = now;

			update(deltaTime);

			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}

	}

	virtual void update(float deltaTime){}

protected:
	float m_window_width = 1600;
	float m_window_height = 800;
	GLFWwindow* m_window = nullptr;
};
