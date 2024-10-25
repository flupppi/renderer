// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com



#include "Game.h"
//************************************
// Set up Keyboard Observer, Initialize the Renderer and Initialize the Skeleton.
//************************************
void Game::Initialize(GLFWwindow* window)
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
void Game::Render(float aspectRatio)
{
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 3.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat transformOpertation = glm::angleAxis(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat model_quat = transformOpertation;
	glm::mat4 Model = glm::mat4_cast(model_quat);
	glm::mat4 mvp = Projection * View * Model;



	glm::mat4 quadTransform = mvp * glm::mat4(1.0f);

	m_renderer.RenderQuad(quadTransform);
}

//************************************
// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
//************************************
void Game::ClearResources()
{
	m_renderer.ClearResources();
}

//************************************
// Handle Input and Update Animation
//************************************
void Game::Update(double deltaTime)
{
	m_input.Update();

	// Keyboard Rotation
	float xVel = 0.0f;
	if (m_input.IsKeyDown(GLFW_KEY_UP))
		std::cout << "pushed a button" << std::endl;

}
