// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once
#include "../GameInterface.h"
#include "Renderer.h"
#include "Joint.h"
#include "Skeleton.h"
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../InputSystem.h"
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

namespace Engine {

	class Application : public GameInterface
	{
	public:
		void Initialize(GLFWwindow* window) override;
		void BuildSkeleton();
		void Render(float aspectRatio) override;
		void RenderIMGui();
		void ClearResources() override;
		void Update(double deltaTime) override;
		Skeleton m_skeleton;
		//PoseManager m_poseManager;
		GLfloat lightPos[3] = { 15.0f, 1.0f, 15.0f };
		GLfloat lightColor[3] = { 1.0f, 1.0f, 1.0f };

	private:
		float t = 0;
		Renderer m_renderer;
		InputSystem m_input;
		float deltaTime_ = 0.0f;
		glm::quat m_orientationQuaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);;
		float m_currentAngle = 0.0f;
		float m_transitionTime = 0.0f;
		const float TRANSITION_DURATION = 10.0f;
		// Our state
		bool edit_state = false;
		bool show_demo_window = true;
		bool show_another_window = false;
		bool show_mesh = false;
	};
}