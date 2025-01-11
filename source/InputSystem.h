// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once
#include "KeyboardObserver.h"
#include <map>
#include <glm/mat4x4.hpp>
#include <GLFW/glfw3.h>
namespace Engine {


	class InputSystem
	{
	public:
		InputSystem() { m_window = nullptr; }
		void SetWindow(GLFWwindow* window) { m_window = window; };
		void Update();
		void ObserveKey(int key);

		bool IsKeyDown(int key) { return m_keyMapper[key].m_isDown; };
		bool WasKeyPressed(int key) { return m_keyMapper[key].m_wasPressed; };
		bool WasKeyReleased(int key) { return m_keyMapper[key].m_wasReleased; };

		bool IsLeftMouseButtonDown();
		bool IsRightMouseButtonDown();

		void GetPickingRay(const glm::mat4& transformationMatrix, glm::vec3& startingPoint, glm::vec3& direction);
		void GetMousePosition(double& xpos, double& ypos);

	private:
		std::map<int, KeyboardObserver> m_keyMapper;
		GLFWwindow* m_window;
	};

}