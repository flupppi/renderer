// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Raytracer.h"

//************************************
// Set up Keyboard Observer, Initialize the Renderer and Initialize the Skeleton.
//************************************
void Raytracer::Initialize(GLFWwindow* window)
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


void Raytracer::GenerateRayTraceImage()
{
	m_rayTraceImage.resize(m_imageWidth * m_imageHeight * 4);  // 4 bytes per pixel for RGBA

	// Example of filling with a gradient (replace this with actual ray tracing logic)
	for (int y = 0; y < m_imageHeight; ++y) {
		for (int x = 0; x < m_imageWidth; ++x) {
			int index = (y * m_imageWidth + x) * 4;
			m_rayTraceImage[index + 0] = (uint8_t)((float)x / m_imageWidth * 255);  // Red
			m_rayTraceImage[index + 1] = (uint8_t)((float)y / m_imageHeight * 255); // Green
			m_rayTraceImage[index + 2] = 128;  // Blue
			m_rayTraceImage[index + 3] = 255;  // Alpha
		}
	}
}

//************************************
// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
//************************************
void Raytracer::Render(float aspectRatio)
{
	glm::mat4 Projection = m_camera.GetProjectionMatrix(aspectRatio);
	glm::mat4 View = m_camera.GetViewMatrix();
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 mvp = Projection * View * Model;

	glm::mat4 quadTransform = mvp * glm::mat4(1.0f);

	// Render the ray traced texture as a full-screen quad
	m_renderer.RenderRayTraceTexture();

	m_renderer.RenderQuad(quadTransform);
	// Render the gizmo lines
	m_renderer.RenderGizmo(mvp);
	RenderIMGui();
}

//************************************
// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
//************************************
void Raytracer::ClearResources()
{
	m_renderer.ClearResources();
}

//************************************
// Handle Input and Update Animation
//************************************
void Raytracer::Update(double deltaTime)
{
	m_input.Update();
	bool rotateLeft = m_input.IsKeyDown(GLFW_KEY_LEFT);
	bool rotateRight = m_input.IsKeyDown(GLFW_KEY_RIGHT);
	bool zoomIn = m_input.IsKeyDown(GLFW_KEY_UP);
	bool zoomOut = m_input.IsKeyDown(GLFW_KEY_DOWN);

	// Update the camera with input flags
	m_camera.Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
	// Generate the ray traced image on the CPU
	GenerateRayTraceImage();

	// Update the GPU texture with the new image
	m_renderer.UpdateTexture(m_rayTraceImage, m_imageWidth, m_imageHeight);

}



void Raytracer::RenderIMGui()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		ImGui::Begin("Raytracing Stats");

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

