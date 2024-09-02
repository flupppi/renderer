// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "InputSystem.h"

void InputSystem::Update()
{
	for (auto i = m_keyMapper.begin(); i != m_keyMapper.end(); ++i)
		i->second.Update();
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



