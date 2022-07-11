#pragma once

#include "QuickMaths.h"

class App;

struct WindowData
{
	int width;
	int height;
	Vec2f mousePos;
	App* app;
};

class App
{
public:
	~App();
	void start();
	WindowData& getWindowData();
	App();

protected:
	virtual void onShutdown();
	virtual void onUpdate();
	virtual void onMouseScrolled(double offsetX, double offsetY) {};
	virtual void onMousePressed(int button) {};
	virtual void onMouseReleased(int button) {};
	virtual void onKeyPressed(int key) {};
	virtual void onKeyReleased(int key) {};
	virtual void onWindowClosed() {};
	virtual void onDrawImGui() {};

protected:
	WindowData windowData_;
	GLFWwindow* window_;
	bool isRunning_ = true;
};
