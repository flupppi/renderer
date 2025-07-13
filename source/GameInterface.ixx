// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

module;
#include <GLFW/glfw3.h>

export module GameInterface;
import std;

namespace Engine {

	export class GameInterface
	{
	public:
		virtual ~GameInterface() = default;

		virtual void Initialize() {};
		virtual void Initialize(GLFWwindow* window) { Initialize(); }

		virtual void Update(double deltaTime) {};
		virtual void Render(float aspectRatio) {};

		virtual void ClearResources() {};

		std::string mode = "Application";
	};
}