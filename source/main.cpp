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
	// Our state
	bool edit_state = false;
	bool show_demo_window = true;
	bool show_another_window = false;
	static bool show_mesh = false;

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

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float xTurn = 0.0f;
			static float yBend = 0.0f;
			static float zBow = 0.0f;
			static float yOpen = 0.0f;
			static float xJut = 0.0f;
			static float zlookUDleft = 0.0f;
			static float ylookLRleft = 0.0f;
			static float zlookUDright = 0.0f;
			static float ylookLRright = 0.0f;
			static float zLtUpperLid = 0.0f;
			static float zLtLowerLid = 0.0f;
			static float zRtUpperLid = 0.0f;
			static float zRtLowerLid = 0.0f;
			static float expression = 0.5f;

			bool save = false;

			ImGui::Begin("Animation Controls");

			ImGui::Text("edit animation poses by moving the sliders");
			ImGui::Checkbox("Edit Animations", &edit_state);      // Edit bools storing our window open/close state
			if (edit_state) {
				static int item_current = 1;

				// Using the _simplified_ one-liner ListBox() api here
				// See "List boxes" section for examples of how to use the more flexible BeginListBox()/EndListBox() api.
				ImGui::Text("Head");
				ImGui::SliderFloat("Turn", &xTurn, -90.0f, 90.0f);
				ImGui::SliderFloat("Bend", &yBend, -40.0f, 40.0f);
				ImGui::SliderFloat("Bow", &zBow, -50.0f, 50.0f);
				ImGui::Text("Jaw");
				ImGui::SliderFloat("Open", &yOpen, -20.0f, 0.0f);
				ImGui::SliderFloat("Jut", &xJut, -8.0f, 8.0f);
				ImGui::Text("Left Eye");							
				ImGui::SliderFloat("Look Up-Down (left)", &zlookUDleft, -25.0f, 25.0f);
				ImGui::SliderFloat("Look Left-Right (left)", &ylookLRleft, -25.0f, 25.0f);
				ImGui::SliderFloat("Open-Close Upper Eye Lid (left)", &zLtUpperLid, -45.0f, 10.0f);
				ImGui::SliderFloat("Open-Close Lower Eye Lid (left)", &zLtLowerLid, -5.0f, 25.0f);
				ImGui::Text("Right Eye");
				ImGui::SliderFloat("Look Up-Down (right)", &zlookUDright, -25.0f, 25.0f);
				ImGui::SliderFloat("Look Left-Right (right)", &ylookLRright, -25.0f, 25.0f);
				ImGui::SliderFloat("Open-Close Upper Eye Lid (right)", &zRtUpperLid, -45.0f, 10.0f);
				ImGui::SliderFloat("Open-Close Lower Eye Lid (right)", &zRtLowerLid, -5.0f, 25.0f);
				ImGui::Text("Expression");
				ImGui::SliderFloat("Smile - Frown", &expression, 0.0f, 1.0f);



				glm::quat MTurn = glm::angleAxis(glm::radians(xTurn), glm::vec3(1.0f, 0.0f, 0.0f));
				glm::quat MBow = glm::angleAxis(glm::radians(yBend), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat MBend = glm::angleAxis(glm::radians(zBow), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::quat MOpen = glm::angleAxis(glm::radians(yOpen), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::quat MJut = glm::angleAxis(glm::radians(xJut), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat MlookUDleft = glm::angleAxis(glm::radians(zlookUDleft), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::quat MlookLRleft = glm::angleAxis(glm::radians(ylookLRleft), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat MlookUDright = glm::angleAxis(glm::radians(zlookUDright), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::quat MlookLRright = glm::angleAxis(glm::radians(ylookLRright), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat LLtUpperLid = glm::angleAxis(glm::radians(zLtUpperLid), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::quat LLtLowerLid = glm::angleAxis(glm::radians(zLtLowerLid), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::quat LRtUpperLid = glm::angleAxis(glm::radians(zRtUpperLid), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::quat LRtLowerLid = glm::angleAxis(glm::radians(zRtLowerLid), glm::vec3(0.0f, 0.0f, 1.0f));


				glm::mat4 LHead = glm::mat4_cast(MTurn * MBend * MBow);
				glm::mat4 LJaw = glm::mat4_cast(MOpen * MJut);
				glm::mat4 LLeftEye = glm::mat4_cast(MlookLRleft * MlookUDleft);
				glm::mat4 LRightEye = glm::mat4_cast(MlookLRright * MlookUDright);
				glm::mat4 LtLowerLid = glm::mat4_cast(LLtLowerLid);
				glm::mat4 LtUpperLid = glm::mat4_cast(LLtUpperLid);
				glm::mat4 RtLowerLid = glm::mat4_cast(LRtLowerLid);
				glm::mat4 RtUpperLid = glm::mat4_cast(LRtUpperLid);


				gApplication.m_skeleton.m_joints[1].m_targetTransformationMatrix = LHead;
				gApplication.m_skeleton.m_joints[6].m_targetTransformationMatrix = LJaw;
				gApplication.m_skeleton.m_joints[7].m_targetTransformationMatrix = LLeftEye;
				gApplication.m_skeleton.m_joints[8].m_targetTransformationMatrix = LRightEye;
				gApplication.m_skeleton.m_joints[4].m_targetTransformationMatrix = LtLowerLid;
				gApplication.m_skeleton.m_joints[2].m_targetTransformationMatrix = LtUpperLid;
				gApplication.m_skeleton.m_joints[5].m_targetTransformationMatrix = RtLowerLid;
				gApplication.m_skeleton.m_joints[3].m_targetTransformationMatrix = RtUpperLid;
				gApplication.m_skeleton.targetExpression = expression;
			}
			{
				ImGui::Text("Light Color");
				ImGui::ColorEdit3("clear color", (float*)&gApplication.lightColor); // Edit 3 floats representing a color
				ImGui::Text("Light Position");
				ImGui::SliderFloat("X-Position", &gApplication.lightPos[0], -25.0f, 25.0f);
				ImGui::SliderFloat("Y-Position", & gApplication.lightPos[1], -25.0f, 25.0f);
				ImGui::SliderFloat("Z-Position", & gApplication.lightPos[2], -25.0f, 25.0f);
			}
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

		double currentTime = glfwGetTime();
		timeDifference = currentTime - lastTime;
		lastTime = currentTime;
	}
}

void RunCoreloop2(GLFWwindow* window)
{


	double lastTime = glfwGetTime();
	double timeDifference = 0.0f;
	// Our state
	bool edit_state = false;
	bool show_demo_window = true;
	bool show_another_window = false;
	static bool show_mesh = false;

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
	gUsedInterface = &gBillboardCloud;
	GLFWwindow* window = InitializeSystem();
	//RunCoreloop(window);
	RunCoreloop2(window);
	ShutdownSystem();
}

