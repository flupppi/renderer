module;
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

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

}