// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once

struct GLFWwindow;
namespace Engine {

	class GameInterface
	{
	public:
		virtual void Initialize() {};
		virtual void Initialize(GLFWwindow* window) { Initialize(); }

		virtual void Update(double deltaTime) {};
		virtual void Render(float aspectRatio) {};

		virtual void ClearResources() {};
	};
}