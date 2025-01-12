// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "BillboardCloud.h"
namespace Engine {
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
		m_input.ObserveKey(GLFW_KEY_J);
		m_input.ObserveKey(GLFW_KEY_K);
		m_input.ObserveKey(GLFW_KEY_I);
		m_input.ObserveKey(GLFW_KEY_M);
		m_renderer.Initialize();

		for (int i = 0; i < 5; i++) {
			Quad quad;
			quad.trans.SetPosition(glm::vec3(-5.0f + (i * 2.1f), 0.0f, 0.0f));
			m_scene.push_back(quad);
			m_renderer.InitQuad(quad);
		}
	}
	//************************************
	// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
	//************************************
	void BillboardCloud::Render(float aspectRatio)
	{
		
		glm::mat4 Projection{ m_camera.GetProjectionMatrix(aspectRatio) };
		glm::mat4 View{ m_camera.GetViewMatrix() };
		// Use the transform matrix from the TransformComponent
		for (auto model : m_scene) {
			glm::mat4 Model{ model.trans.GetTransform() };

			glm::mat4 mvp{ Projection * View * Model };
			m_renderer.RenderQuad(mvp);

		}
		
		// Render the gizmo lines
		m_renderer.RenderGizmo(Projection * glm::mat4(1.0f) * View);
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
		bool rotateLeft{ m_input.IsKeyDown(GLFW_KEY_LEFT) };
		bool rotateRight{ m_input.IsKeyDown(GLFW_KEY_RIGHT) };
		bool zoomIn{ m_input.IsKeyDown(GLFW_KEY_UP) };
		bool zoomOut{ m_input.IsKeyDown(GLFW_KEY_DOWN) };

		bool right{ m_input.IsKeyDown(GLFW_KEY_K) };
		bool left{ m_input.IsKeyDown(GLFW_KEY_J) };
		bool up{ m_input.IsKeyDown(GLFW_KEY_I) };
		bool down{ m_input.IsKeyDown(GLFW_KEY_M) };
		if (right) {
			glm::vec3 deltaPos{1.0f*deltaTime, 0.0f, 0.0f };
			glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
			glm::vec3 newPos{ oldPos + deltaPos };
			m_scene.at(0).trans.SetPosition(newPos);
		}
		if (left) {
			glm::vec3 deltaPos{-1.0f*deltaTime, 0.0f, 0.0f };
			glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
			glm::vec3 newPos{ oldPos + deltaPos };
			m_scene.at(0).trans.SetPosition(newPos);
		}
		if (up) {
			glm::vec3 deltaPos{0.0f, 1.0f*deltaTime, 0.0f };
			glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
			glm::vec3 newPos{ oldPos + deltaPos };
			m_scene.at(0).trans.SetPosition(newPos);
		}
		if (down) {
			glm::vec3 deltaPos{0.0f, -1.0f*deltaTime, 0.0f };
			glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
			glm::vec3 newPos{ oldPos + deltaPos };
			m_scene.at(0).trans.SetPosition(newPos);
		}


		
		// Update the camera with input flags
		m_camera.Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
	}
}