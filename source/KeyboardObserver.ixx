// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#include "GLFW/glfw3.h"
export module KeyboardObserver;
namespace Engine {

	export class KeyboardObserver {
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

	
	KeyboardObserver::KeyboardObserver() : KeyboardObserver(nullptr, -1){}

	KeyboardObserver::KeyboardObserver(GLFWwindow* window, int key):m_window(window),m_key(key), m_isDown(false),
		m_wasPressed(false), m_wasReleased(false){}
	void KeyboardObserver::Update() {
		bool isDown = (glfwGetKey(m_window, m_key) == GLFW_PRESS);

		m_wasPressed = isDown && (!m_isDown);
		m_wasReleased = (!isDown) && m_isDown;
		m_isDown = isDown;
	}

}