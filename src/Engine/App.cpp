#include "../pch.h"
#include "App.h"
#include "QuickMaths.h"
#include "Time.h"
#include "ImGuiHandler.h"
#include "Renderer2D.h"

void onWindowResize(GLFWwindow * m_window, int w, int h)
{
	glViewport(0, 0, w, h);
}

App::App()
{
	glfwSetErrorCallback([](int errorCode, const char* desc) { LOG("GLFW error: {}", desc); });

	if (!glfwInit())
		LOG("Failed to init GLFW!");

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
 
	/*
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	*/
 
	glfwWindowHint(GLFW_MAXIMIZED, true);
	window_ = glfwCreateWindow(1920, 1080, "Project \"Alive\"", NULL, NULL);

	windowData_.app = this;
	windowData_.width = 1920;
	windowData_.height = 1080;

	//window_ = glfwCreateWindow(windowData_.width, windowData_.height, "Project Alive", nullptr, nullptr);
	if (window_ == NULL)
	{
		LOG("Failed to create GLFW window!");
		glfwTerminate();
		return;
	}

	glfwSetWindowUserPointer(window_, &windowData_);

	glfwMakeContextCurrent(window_);
	glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int w, int h)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->width = w;
		data->height = h;
		glViewport(0, 0, w, h);
	});

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG("Failed to initialize GLAD!");
		return;
	}

	glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double posX, double posY)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->mousePos = Vec2f(posX / data->width, posY / data->height);
	});

	glfwSetScrollCallback(window_, [](GLFWwindow* window, double offsetX, double offsetY)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->app->onMouseScrolled(offsetX, offsetY);
	});

	glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				data->app->onMousePressed(button);
				break;
			}
			case GLFW_RELEASE:
			{
				data->app->onMouseReleased(button);
				break;
			}
		}
	});

	glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

		switch (action)
		{
			case GLFW_PRESS:
			{
				data->app->onKeyPressed(key);
				break;
			}
			case GLFW_RELEASE:
			{
				data->app->onKeyReleased(key);
				break;
			}
		}
	});

	glfwSetWindowCloseCallback(window_, [](GLFWwindow* window)
	{
		auto* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
		data->app->onWindowClosed();
	});

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

App::~App()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window_);
	glfwTerminate();
}

void App::start()
{
	while (!glfwWindowShouldClose(window_))
	{	
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		onDrawImGui();

		ImGui::Render();
        int displayW, displayH;
        glfwGetFramebufferSize(window_, &displayW, &displayH);
        //glViewport(0, 0, display_w, display_h);
		Renderer2D::clear();		
		
		static clock_t start = clock();
		clock_t now = clock();
		float deltaSeconds = ((now - start) / (float)CLOCKS_PER_SEC);
		Time::DeltaSeconds = deltaSeconds * Time::Scale;
		Time::DeltaMillis = deltaSeconds * 1000 * Time::Scale;
		start = now;
		Time::ElapsedSeconds += Time::DeltaSeconds;
		Time::ElapsedSecondsUnscaled += deltaSeconds;

		onUpdate();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    	
        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

		glfwSwapBuffers(window_);
	}

	onShutdown();
}

WindowData& App::getWindowData()
{
	return windowData_;
}

void App::onShutdown()
{
}

void App::onUpdate()
{
}
