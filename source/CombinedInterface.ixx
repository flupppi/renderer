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
import Game;

namespace Engine {

	// Global or member variables for tool visibility:
	static bool showAnimationControls = true;
	static bool showSemanticVisualization = true;

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
		Game game;
	};
	void CombinedInterface::Initialize(GLFWwindow* window)
	{
		vis.Initialize(window);
		app.Initialize(window);
		game.Initialize(window);
	}
	void CombinedInterface::Render(float aspectRatio)
	{
		// 1) Clear the screen once.
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Query the actual framebuffer size for correct viewport sizes
		int screenWidth, screenHeight;
		glfwGetFramebufferSize(glfwGetCurrentContext(), &screenWidth, &screenHeight);

		// 2) Render the "Application" (3D Scene) on the LEFT half

		int leftWidth = screenWidth / 2;
		int leftHeight = screenHeight;

		// Set the viewport to [0..leftWidth, 0..leftHeight]
		glViewport(0, 0, leftWidth, leftHeight);

		// Compute an aspect ratio for that sub-viewport
		float leftAspect = static_cast<float>(leftWidth) / static_cast<float>(leftHeight);

		// Then let your "app" do its 3D rendering
		app.Render(leftAspect);


		// 3) Render the "Game" (2D Scene) on the RIGHT half

		int rightWidth = screenWidth / 2;
		int rightHeight = screenHeight;

		// The right side starts at X = leftWidth
		int xOffset = screenWidth / 2;
		glViewport(xOffset, 0, rightWidth, rightHeight);

		float rightAspect = static_cast<float>(rightWidth) / static_cast<float>(rightHeight);

		// Then let your "game" do its 2D rendering
		game.Render(rightAspect);


		// 4) Reset the viewport to the FULL window before ImGui
		glViewport(0, 0, screenWidth, screenHeight);


		// 5) Begin an ImGui frame EXACTLY once for the entire app
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{   // We want to draw a rectangle around the left half and around the right half.
			ImDrawList* drawList = ImGui::GetBackgroundDrawList();

			ImU32 borderColor = IM_COL32(255, 255, 255, 255);
			float thickness = 2.0f;

			// Left rectangle: top-left(0,0), bottom-right(leftWidth, screenHeight)
			drawList->AddRect(
				ImVec2(0, 0),
				ImVec2((float)leftWidth, (float)screenHeight),
				borderColor,
				0.0f,    // rounding
				0,       // corner flags
				thickness
			);

			// Right rectangle: top-left(leftWidth,0), bottom-right(screenWidth, screenHeight)
			drawList->AddRect(
				ImVec2((float)leftWidth, 0.0f),
				ImVec2((float)screenWidth, (float)screenHeight),
				borderColor,
				0.0f,
				0,
				thickness
			);
		}

		// Define Menu Bar
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					// Handle exit logic
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				// Toggle tool windows via boolean flags.
				ImGui::MenuItem("Animation Controls", nullptr, &showAnimationControls);
				ImGui::MenuItem("Semantic Visualization", nullptr, &showSemanticVisualization);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		// 6) Let the "SemanticVisualization" build its ImGui windows
		//    (this draws purely in ImGui, no sub-viewport needed)
		app.DrawImGui(&showAnimationControls);
		vis.DrawImGui(&showSemanticVisualization);

		// 7) End the ImGui frame EXACTLY once
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void CombinedInterface::ClearResources()
	{
		vis.ClearResources();
		app.ClearResources();
		game.ClearResources();
	}
	void CombinedInterface::Update(double deltaTime)
	{
		vis.Update(deltaTime);
		app.Update(deltaTime);
		game.Update(deltaTime);
	}

}
