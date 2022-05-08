#pragma once

#include "QuickMaths.h"

class App;

struct WindowData
{
	int Width;
	int Height;
	Vec2 MousePos;
	App* App;
};

class App
{
public:
	App();
	void start();
protected:
	virtual void update();
	virtual void mouse_scrolled(double x_offset, double y_offset) {};
	virtual void mouse_pressed(int button) {};
	virtual void mouse_released(int button) {};
	virtual void key_pressed(int key) {};
	virtual void key_released(int key) {};
	virtual void window_closed() {};
protected:
	WindowData m_window_data;
	GLFWwindow* m_window;
	clock_t m_frame_start;
};
