module;
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

export module Game;
import std;
import GameInterface;
import GameRenderer;
import InputSystem;
import Quad;

namespace Engine {

	export class Game : public GameInterface
	{
	public:
		void Initialize(GLFWwindow* window) override;
		void Render(float aspectRatio) override;
		void ClearResources() override;
		void Update(double deltaTime) override;
		void RenderIMGui();
	private:
		GameRenderer m_renderer;
		InputSystem m_input;

		glm::vec2 m_playerPosition{ 0.0f, 0.0f }; // Player quad position
		glm::vec2 m_collectiblePosition{ 3.0f, 3.0f }; // Random collectible position
		float m_playerSpeed{ 5.0f }; // Movement speed
		int m_score{ 0 }; // Player's score
	};

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


		Quad playerQuad(glm::vec3(m_playerPosition, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f));
		m_renderer.InitQuad(playerQuad);
	}
	//************************************
	// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
	//************************************
	void Game::Render(float aspectRatio)
	{
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
		glm::mat4 View = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Render Player
		glm::mat4 ModelPlayer = glm::translate(glm::mat4(1.0f), glm::vec3(m_playerPosition, 0.0f));
		m_renderer.RenderQuad(Projection * View * ModelPlayer, 0);

		// Render Collectible
		glm::mat4 ModelColletible = glm::translate(glm::mat4(1.0f), glm::vec3(m_collectiblePosition, 0.0f));
		m_renderer.RenderQuad(Projection * View * ModelColletible, 1);

		RenderIMGui();

	}

	void Game::RenderIMGui() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Game HUD");
			ImGui::Text("Score: %d", m_score);
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

		glm::vec2 direction(0.0f);
		if (m_input.IsKeyDown(GLFW_KEY_UP))
			direction.y += 1.0f;
		if (m_input.IsKeyDown(GLFW_KEY_DOWN))
			direction.y -= 1.0f;
		if (m_input.IsKeyDown(GLFW_KEY_LEFT))
			direction.x -= 1.0f;
		if (m_input.IsKeyDown(GLFW_KEY_RIGHT))
			direction.x += 1.0f;

		m_playerPosition += direction * m_playerSpeed * static_cast<float>(deltaTime);

		if (glm::length(m_playerPosition - m_collectiblePosition) < 0.5f) {
			m_score++;
			m_collectiblePosition = glm::vec2(rand() % 10 - 5, rand() % 10 - 5); // Randomize position
			std::cout << "Score: " << m_score << std::endl;
		}

	}

}