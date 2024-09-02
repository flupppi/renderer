// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once
#include "GLFW/glfw3.h"
struct GLFWwindow;
class KeyboardObserver{
private:
	GLFWwindow* m_window;
	int m_key;

public:
	KeyboardObserver();
	KeyboardObserver(GLFWwindow* window, int key);
	void Update();

	bool m_isDown;
	bool m_wasPressed;
	bool m_wasReleased;
};

