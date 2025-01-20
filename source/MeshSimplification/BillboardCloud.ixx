module;
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>

export module BillboardCloud;
import std;
import GameInterface;
import IBillboardGenerator;
import IPlaneSelector;
import BillboardRenderer;
import InputSystem;
import Camera;
import Quad;
namespace Engine {
	
	export class BillboardCloud : public GameInterface
	{
	public:
		BillboardCloud(std::unique_ptr<IBillboardGenerator> generator,
			std::unique_ptr<IPlaneSelector> selector)
			: m_billboardGenerator(std::move(generator)),
			m_planeSelector(std::move(selector)) {
		}
		void Initialize(GLFWwindow* window) override;
		void Render(float aspectRatio) override;
		void ClearResources() override;
		void Update(double deltaTime) override;

	private:
		BillboardRenderer m_renderer;
		InputSystem m_input;
		Camera m_camera;
		std::vector<Quad> m_scene;
		const std::unique_ptr<IBillboardGenerator> m_billboardGenerator;
		const std::unique_ptr<IPlaneSelector> m_planeSelector;

	};
}

