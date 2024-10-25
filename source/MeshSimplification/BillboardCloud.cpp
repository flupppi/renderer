// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "BillboardCloud.h"

//************************************
// Set up Keyboard Observer, Initialize the Renderer and Initialize the Skeleton.
//************************************
void BillboardCloud::Initialize(GLFWwindow* window)
{
	m_input.SetWindow(window);
	// Observe Input
	m_input.ObserveKey(GLFW_KEY_SPACE);
	m_input.ObserveKey(GLFW_KEY_RIGHT);
	m_input.ObserveKey(GLFW_KEY_LEFT);
	m_input.ObserveKey(GLFW_KEY_UP);
	m_input.ObserveKey(GLFW_KEY_DOWN);
	m_input.ObserveKey(GLFW_KEY_9);
	m_input.ObserveKey(GLFW_KEY_8);
	m_input.ObserveKey(GLFW_KEY_7);
	m_input.ObserveKey(GLFW_KEY_6);
	m_input.ObserveKey(GLFW_KEY_4);
	m_input.ObserveKey(GLFW_KEY_3);
	m_input.ObserveKey(GLFW_KEY_2);
	m_input.ObserveKey(GLFW_KEY_1);
	m_input.ObserveKey(GLFW_KEY_R);
	m_input.ObserveKey(GLFW_KEY_LEFT_SHIFT);


	m_renderer.Initialize();
	Quad quad(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f));
	m_renderer.InitQuad(quad);
}
//************************************
// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
//************************************
void BillboardCloud::Render(float aspectRatio)
{
	glm::mat4 Projection = m_camera.GetProjectionMatrix(aspectRatio);
	glm::mat4 View = m_camera.GetViewMatrix();
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 mvp = Projection * View * Model;

	glm::mat4 quadTransform = mvp * glm::mat4(1.0f);
	m_renderer.RenderQuad(quadTransform);
	// Render the gizmo lines
	m_renderer.RenderGizmo(mvp);
}

//************************************
// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
//************************************
void BillboardCloud::ClearResources()
{
	m_renderer.ClearResources();
}

//************************************
// Handle Input and Update Animation
//************************************
void BillboardCloud::Update(double deltaTime)
{
	m_input.Update();
	bool rotateLeft = m_input.IsKeyDown(GLFW_KEY_LEFT);
	bool rotateRight = m_input.IsKeyDown(GLFW_KEY_RIGHT);
	bool zoomIn = m_input.IsKeyDown(GLFW_KEY_UP);
	bool zoomOut = m_input.IsKeyDown(GLFW_KEY_DOWN);

	// Update the camera with input flags
	m_camera.Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);

}
