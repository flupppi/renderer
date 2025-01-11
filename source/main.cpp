// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once
#include "GameInterface.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "SkinnedAnimation/Application.h"
#include "MeshSimplification/BillboardCloud.h"
#include "2DGame/Game.h"
#include "Raytracer/Raytracer.h"
#include "MeshSimplification/BillboardGenerator.h"
#include "MeshSimplification/PlaneSelector.h"

using namespace Engine;
GameInterface* gUsedInterface;
const int WIDTH{ 1024 };
const int HEIGHT{ 768 };


// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


//************************************
// Initialize Dear ImGui into the existing GLFW / OpenGL session.
//************************************
void InitializeDearImGui(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(nullptr);
	ImGui::StyleColorsDark();
}

//************************************
// Initialize GLFW, Load OpenGL with glew and set up the renderer.
//************************************
GLFWwindow* InitializeSystem(const std::string& mode)
{
	// Setup window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, mode.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1); // Enable vsync
	glewExperimental = GL_TRUE;
	glewInit();
	InitializeDearImGui(window);

	gUsedInterface->Initialize(window);
	return window;
}

//************************************
// Set up Game Loop and render scene + Process and Draw UI
//************************************
void RunCoreloop(GLFWwindow* window)
{
	double lastTime = glfwGetTime();
	double timeDifference = 0.0f;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		gUsedInterface->Update(timeDifference);
		int screenWidth, screenHeight;
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		float aspectRatio = (float)screenWidth / (float)screenHeight;
		glViewport(0, 0, screenWidth, screenHeight);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gUsedInterface->Render(aspectRatio);

		glfwSwapBuffers(window);

		double currentTime = glfwGetTime();
		timeDifference = currentTime - lastTime;
		lastTime = currentTime;
	}
}

std::unique_ptr<GameInterface> CreateGameInterface(const std::string& mode) {
	if (mode == "BillboardCloud") {
		// Create specific implementations for generator and selector
		auto generator = std::make_unique<BillboardGenerator>();
		auto selector = std::make_unique<PlaneSelector>();
		return std::make_unique<BillboardCloud>(std::move(generator), std::move(selector));
	}
	else if (mode == "Application") {
		return std::make_unique<Application>();
	}
	else if (mode == "Game") {
		return std::make_unique<Game>();
	}
	else if (mode == "Raytracer") {
		return std::make_unique<Raytracer>();
	}
	else {
		throw std::runtime_error("Unknown game mode: " + mode);
	}
}

//************************************
// Unload all resources used by the System.
//************************************
void ShutdownSystem()
{
	gUsedInterface->ClearResources();
	glfwTerminate();
}

//************************************
// Program entry point
//************************************
int main(int argc, char* argv[])
{
	try {
		// Select the game mode based on some configuration, argument, or state.
		const std::string mode = argc > 1 ? argv[1] : "Raytracer";

		// Create the GameInterface instance using the factory function
		const auto gameInterface = CreateGameInterface(mode);

		// Set the global pointer to the created instance
		gUsedInterface = gameInterface.get();  // Assign to the global pointer

		GLFWwindow* window = InitializeSystem(mode);
		RunCoreloop(window);
		ShutdownSystem();
	}
	catch (std::out_of_range&)
	{
		std::cerr << "range error\n";
	}
	catch (...)
	{
		std::cerr << "unknown exception thrown\n";
	}
}