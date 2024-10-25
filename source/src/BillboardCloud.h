#pragma once
#include "../GameInterface.h"
#include "BillboardRenderer.h"
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include "../InputSystem.h"
#include "../Quad.h"
#include "../Camera.h"

class BillboardCloud : public GameInterface
{
public:
	void Initialize(GLFWwindow* window) override;
	void Render(float aspectRatio) override;
	void ClearResources() override;
	void Update(double deltaTime) override;
private:
	BillboardRenderer m_renderer;
	InputSystem m_input;
	Camera m_camera;

};

