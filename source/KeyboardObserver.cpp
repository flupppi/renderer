module;
#include "GLFW/glfw3.h"
module KeyboardObserver;


namespace Engine {
	KeyboardObserver::KeyboardObserver() : KeyboardObserver(nullptr, -1)
	{

	}

	KeyboardObserver::KeyboardObserver(GLFWwindow* window, int key)
	{
		m_window = window;
		m_key = key;

		m_isDown = false;
		m_wasPressed = false;
		m_wasReleased = false;
	}

	void KeyboardObserver::Update()
	{
		bool isDown = (glfwGetKey(m_window, m_key) == GLFW_PRESS);

		m_wasPressed = isDown && (!m_isDown);
		m_wasReleased = (!isDown) && m_isDown;
		m_isDown = isDown;
	}
}

