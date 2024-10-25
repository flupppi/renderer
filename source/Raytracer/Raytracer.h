#pragma once
#include "../GameInterface.h"
#include "RaytracerRenderer.h"
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include "../InputSystem.h"
#include "../Quad.h"
#include "../Camera.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

class Raytracer : public GameInterface
{
public:
	void Initialize(GLFWwindow* window) override;
	void Render(float aspectRatio) override;
	void ClearResources() override;
	void Update(double deltaTime) override;
	void RenderIMGui();
private:
	RaytracerRenderer m_renderer;
	InputSystem m_input;
	Camera m_camera;

	// Image buffer for ray tracing output
	std::vector<uint8_t> m_rayTraceImage;
	int m_imageWidth = 1024;
	int m_imageHeight = 768;

	void GenerateRayTraceImage(); // Ray tracing function


};

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

struct Sphere {
	glm::vec3 center;
	float radius;
};

