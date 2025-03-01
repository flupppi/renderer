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
export module Raytracer;
import std;
import GameInterface;
import Camera;
import Quad;
import InputSystem;
import RaytracerRenderer;
namespace Engine {

	export class Raytracer : public GameInterface
	{
	public:
		void Initialize(GLFWwindow* window) override;
		void Render(float aspectRatio) override;
		void ClearResources() override;
		void Update(double deltaTime) override;
		void RenderIMGui();
	private:
		RaytracerRenderer m_renderer;
		InputSystem m_input;
		Camera m_camera;

		// Image buffer for ray tracing output
		std::vector<uint8_t> m_rayTraceImage;
		int m_imageWidth{ 1024 };
		int m_imageHeight{ 768 };

		void GenerateRayTraceImage(); // Ray tracing function


	};

	struct Ray {
		glm::vec3 origin;
		glm::vec3 direction;
	};

	struct Sphere {
		glm::vec3 center;
		float radius;
	};

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

	bool IntersectSphere(const Ray& ray, const Sphere& sphere, float& t)
	{
		glm::vec3 oc = ray.origin - sphere.center;
		float a = glm::dot(ray.direction, ray.direction);
		float b = 2.0f * glm::dot(oc, ray.direction);
		float c = glm::dot(oc, oc) - sphere.radius * sphere.radius;
		float discriminant = b * b - 4 * a * c;

		if (discriminant < 0) {
			return false; // No intersection
		}
		else {
			t = (-b - sqrt(discriminant)) / (2.0f * a);
			return true;
		}
	}
	void Raytracer::GenerateRayTraceImage()
	{
		m_rayTraceImage.resize(m_imageWidth * m_imageHeight * 4);  // 4 bytes per pixel for RGBA

		// Define camera properties
		glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -5.0f);  // Camera is 5 units back along z-axis
		float viewportHeight = 2.0f;
		float viewportWidth = (float)m_imageWidth / m_imageHeight * viewportHeight;
		float focalLength = 1.0f;

		// Define sphere properties
		Sphere sphere;
		sphere.center = glm::vec3(0.0f, 0.0f, 0.0f); // Sphere at the origin
		sphere.radius = 0.5f;

		// Calculate the image plane
		glm::vec3 horizontal = glm::vec3(viewportWidth, 0.0f, 0.0f);
		glm::vec3 vertical = glm::vec3(0.0f, viewportHeight, 0.0f);
		glm::vec3 lowerLeftCorner = cameraPosition - horizontal / 2.0f - vertical / 2.0f + glm::vec3(0.0f, 0.0f, focalLength);

		// Loop through each pixel
		for (int y = 0; y < m_imageHeight; ++y) {
			for (int x = 0; x < m_imageWidth; ++x) {
				int index = (y * m_imageWidth + x) * 4;

				// Compute ray direction
				float u = (float)x / (m_imageWidth - 1);
				float v = (float)y / (m_imageHeight - 1);
				glm::vec3 direction = glm::normalize(lowerLeftCorner + u * horizontal + v * vertical - cameraPosition);

				Ray ray;
				ray.origin = cameraPosition;
				ray.direction = direction;

				// Test for intersection with the sphere
				float t;
				if (IntersectSphere(ray, sphere, t)) {
					// If the ray hits the sphere, color the pixel
					glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);  // Red color for the sphere
					m_rayTraceImage[index + 0] = (uint8_t)(color.r * 255);
					m_rayTraceImage[index + 1] = (uint8_t)(color.g * 255);
					m_rayTraceImage[index + 2] = (uint8_t)(color.b * 255);
					m_rayTraceImage[index + 3] = 255;
				}
				else {
					// Background color
					glm::vec3 color = glm::vec3(0.5f, 0.7f, 1.0f);  // Sky blue
					m_rayTraceImage[index + 0] = (uint8_t)(color.r * 255);
					m_rayTraceImage[index + 1] = (uint8_t)(color.g * 255);
					m_rayTraceImage[index + 2] = (uint8_t)(color.b * 255);
					m_rayTraceImage[index + 3] = 255;
				}
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



}
