// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#include <glm/mat4x4.hpp>
#include <GLFW/glfw3.h>
export module InputSystem;
import std;
import KeyboardObserver;
namespace Engine {


	export class InputSystem
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

		double GetMouseDeltaX() const {
			return m_mouseDeltaX;
		}

		double GetMouseDeltaY() const {
			return m_mouseDeltaY;
		}

	private:
		double m_prevMouseX = 0.0;
		double m_prevMouseY = 0.0;
		double m_mouseDeltaX = 0.0;
		double m_mouseDeltaY = 0.0;
		std::map<int, KeyboardObserver> m_keyMapper;
		GLFWwindow* m_window;
	};


	void InputSystem::Update()
	{
		// Skip processing if the window is not focused
		if (!glfwGetWindowAttrib(m_window, GLFW_FOCUSED)) {
			m_mouseDeltaX = 0.0;
			m_mouseDeltaY = 0.0;
			return;
		}

		// Get the current cursor position
		double currentMouseX, currentMouseY;
		glfwGetCursorPos(m_window, &currentMouseX, &currentMouseY);

		// Get window size and position
		int windowX, windowY, windowWidth, windowHeight;
		glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
		glfwGetWindowPos(m_window, &windowX, &windowY);

		//// Check if cursor is inside the window
		//if (currentMouseX < 0 || currentMouseY < 0 ||
		//	currentMouseX >= windowWidth || currentMouseY >= windowHeight) {
		//	m_mouseDeltaX = 0.0;
		//	m_mouseDeltaY = 0.0;
		//	return;
		//}

		// Calculate mouse deltas
		m_mouseDeltaX = currentMouseX - m_prevMouseX;
		m_mouseDeltaY = currentMouseY - m_prevMouseY;

		m_prevMouseX = currentMouseX;
		m_prevMouseY = currentMouseY;

		// Update key states
		for (auto& [key, observer] : m_keyMapper)
			observer.Update();
	}

	void InputSystem::ObserveKey(int key)
	{
		m_keyMapper[key] = KeyboardObserver(m_window, key);
	}

	bool InputSystem::IsLeftMouseButtonDown()
	{
		return (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	}

	bool InputSystem::IsRightMouseButtonDown()
	{
		return (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	}

	void InputSystem::GetPickingRay(const glm::mat4& transformationMatrix, glm::vec3& startingPoint, glm::vec3& direction)
	{
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);

		int screenWidth, screenHeight;
		glfwGetFramebufferSize(m_window, &screenWidth, &screenHeight);

		xpos = (xpos / screenWidth) * 2.0 - 1.0;
		ypos = 1.0 - 2.0 * (ypos / screenHeight);

		glm::vec4 nearPoint = glm::vec4((float)xpos, (float)ypos, 0.01f, 1.0f);
		glm::vec4 farPoint = nearPoint;
		farPoint.z = 0.99f;

		glm::mat4 inverse = glm::inverse(transformationMatrix);
		nearPoint = inverse * nearPoint;
		farPoint = inverse * farPoint;

		nearPoint /= nearPoint.w;
		farPoint /= farPoint.w;

		startingPoint = nearPoint;
		direction = farPoint - nearPoint;

		direction = glm::normalize(direction);
	}

	void InputSystem::GetMousePosition(double& xpos, double& ypos)
	{
		glfwGetCursorPos(m_window, &xpos, &ypos);
	}


}