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
	virtual void on_mouse_scrolled(double x_offset, double y_offset) {};
	virtual void on_mouse_pressed(int button) {};
	virtual void on_mouse_released(int button) {};
	virtual void on_key_pressed(int key) {};
	virtual void on_key_released(int key) {};
	virtual void on_window_close() {};
protected:
	WindowData m_window_data;
	GLFWwindow* m_window;
};
