module;
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


export module CombinedInterface;

import std;
import GameInterface;
import SemanticVisualization;
import Application;

namespace Engine {

	export class CombinedInterface : public GameInterface
	{
	public:
		void Initialize(GLFWwindow* window) override;
		void Render(float aspectRatio) override;
		void ClearResources() override;
		void Update(double deltaTime) override;
	private:
		SemanticVisualization vis;
		Application app;
	};
	void CombinedInterface::Initialize(GLFWwindow* window)
	{
		vis.Initialize(window);
		app.Initialize(window);
	}
	void CombinedInterface::Render(float aspectRatio)
	{
		// CombinedInterface::Render(...)
		// 1) Begin the ImGui frame exactly once
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		vis.Render(aspectRatio);
		app.Render(aspectRatio);

		// 3) End the ImGui frame exactly once
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



	}
	void CombinedInterface::ClearResources()
	{
		vis.ClearResources();
		app.ClearResources();
	}
	void CombinedInterface::Update(double deltaTime)
	{
		vis.Update(deltaTime);
		app.Update(deltaTime);
	}

}
