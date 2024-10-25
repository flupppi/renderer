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
#include "Application.h"
#include "BillboardCloud.h"
BillboardCloud gBillboardCloud;
Application gApplication;
GameInterface* gUsedInterface;


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
GLFWwindow* InitializeSystem()
{
	// Setup window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Billboard Clouds", nullptr, nullptr);

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
int main()
{
	//gUsedInterface = &gBillboardCloud;
	gUsedInterface = &gApplication;
	GLFWwindow* window = InitializeSystem();
	RunCoreloop(window);
	ShutdownSystem();
}

