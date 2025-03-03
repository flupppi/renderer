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



	//************************************
// Set up Keyboard Observer, Initialize the Renderer and Initialize the Skeleton.
//************************************
	void CombinedInterface::Initialize(GLFWwindow* window)
	{
		vis.Initialize(window);
		app.Initialize(window);
	}


	//************************************
	// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
	//************************************
	void CombinedInterface::Render(float aspectRatio)
	{
		vis.Render(aspectRatio);
		app.Render(aspectRatio);
	}

	//************************************
	// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
	//************************************
	void CombinedInterface::ClearResources()
	{
		vis.ClearResources();
		app.ClearResources();
	}


	//************************************
	// Handle Input and Update Animation
	//************************************
	void CombinedInterface::Update(double deltaTime)
	{
		vis.Update(deltaTime);
		app.Update(deltaTime);
	}

}
