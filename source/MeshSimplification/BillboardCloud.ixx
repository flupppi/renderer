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
		void Initialize(GLFWwindow* window) override {

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
			m_input.ObserveKey(GLFW_KEY_J);
			m_input.ObserveKey(GLFW_KEY_K);
			m_input.ObserveKey(GLFW_KEY_I);
			m_input.ObserveKey(GLFW_KEY_M);

			// Movement Keys
			m_input.ObserveKey(GLFW_KEY_W);
			m_input.ObserveKey(GLFW_KEY_A);
			m_input.ObserveKey(GLFW_KEY_S);
			m_input.ObserveKey(GLFW_KEY_D);
			m_input.ObserveKey(GLFW_KEY_LEFT_ALT);

			m_renderer.Initialize();

			for (int i = 0; i < 5; i++) {
				Quad quad;
				quad.trans.SetPosition(glm::vec3(-5.0f + (i * 2.1f), 0.0f, 0.0f));
				m_scene.push_back(quad);
				m_renderer.InitQuad(quad);
			}
		}
		void Render(float aspectRatio) override {

			glm::mat4 Projection{ m_camera.GetProjectionMatrix(aspectRatio) };
			glm::mat4 View{ m_camera.GetViewMatrix() };
			// Use the transform matrix from the TransformComponent
			for (auto model : m_scene) {
				glm::mat4 Model{ model.trans.GetTransform() };

				glm::mat4 mvp{ Projection * View * Model };
				m_renderer.RenderQuad(mvp);

			}

			// Render the gizmo lines
			m_renderer.RenderGizmo(Projection * glm::mat4(1.0f) * View);
			RenderIMGui();
		}
		void ClearResources() override {
			m_renderer.ClearResources();
		}
		void Update(double deltaTime) override {

			m_input.Update();
			bool rotateLeft{ m_input.IsKeyDown(GLFW_KEY_LEFT) };
			bool rotateRight{ m_input.IsKeyDown(GLFW_KEY_RIGHT) };
			bool zoomIn{ m_input.IsKeyDown(GLFW_KEY_UP) };
			bool zoomOut{ m_input.IsKeyDown(GLFW_KEY_DOWN) };

			bool right{ m_input.IsKeyDown(GLFW_KEY_K) };
			bool left{ m_input.IsKeyDown(GLFW_KEY_J) };
			bool up{ m_input.IsKeyDown(GLFW_KEY_I) };
			bool down{ m_input.IsKeyDown(GLFW_KEY_M) };
			if (right) {
				glm::vec3 deltaPos{ 1.0f * deltaTime, 0.0f, 0.0f };
				glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
				glm::vec3 newPos{ oldPos + deltaPos };
				m_scene.at(0).trans.SetPosition(newPos);
			}
			if (left) {
				glm::vec3 deltaPos{ -1.0f * deltaTime, 0.0f, 0.0f };
				glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
				glm::vec3 newPos{ oldPos + deltaPos };
				m_scene.at(0).trans.SetPosition(newPos);
			}
			if (up) {
				glm::vec3 deltaPos{ 0.0f, 1.0f * deltaTime, 0.0f };
				glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
				glm::vec3 newPos{ oldPos + deltaPos };
				m_scene.at(0).trans.SetPosition(newPos);
			}
			if (down) {
				glm::vec3 deltaPos{ 0.0f, -1.0f * deltaTime, 0.0f };
				glm::vec3 oldPos{ m_scene.at(0).trans.GetPosition() };
				glm::vec3 newPos{ oldPos + deltaPos };
				m_scene.at(0).trans.SetPosition(newPos);
			}
			if (m_input.WasKeyPressed(GLFW_KEY_LEFT_ALT)) {
				m_camera.ToggleMode();
			}


			if (m_camera.GetCameraMode() == Camera::CameraMode::FPS) {
				if (m_input.IsRightMouseButtonDown())
					m_camera.UpdateDirection(m_input.GetMouseDeltaX(), m_input.GetMouseDeltaY());
				else
					m_camera.UpdateDirection(0, 0);

			}

			m_camera.HandleInput(deltaTime,
				m_input.IsKeyDown(GLFW_KEY_W), m_input.IsKeyDown(GLFW_KEY_S),
				m_input.IsKeyDown(GLFW_KEY_A), m_input.IsKeyDown(GLFW_KEY_D),
				rotateLeft, rotateRight,
				zoomIn, zoomOut);

			// Update the camera with input flags
			m_camera.Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
		}

		void RenderIMGui() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			{
				ImGui::Begin("Game HUD");
				ImGui::Text("Render Mode: %s", m_camera.DebugMode().c_str());
			}
			ImGui::End();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	private:
		BillboardRenderer m_renderer;
		InputSystem m_input;
		Camera m_camera;
		std::vector<Quad> m_scene;
		const std::unique_ptr<IBillboardGenerator> m_billboardGenerator;
		const std::unique_ptr<IPlaneSelector> m_planeSelector;

	};
}

