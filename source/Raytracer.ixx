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
	struct Ray {
		glm::vec3 A;
		glm::vec3 B;
		Ray(){}
		Ray(const glm::vec3& a, const glm::vec3& b){A = a; B = b;}
		glm::vec3 origin() const {return A;}
		glm::vec3 direction() const {return B;}
		glm::vec3 point_at_paramter(float t) const {return A + t * B;}
	};

	struct HitRecord {
		float t;
		glm::vec3 p;
		glm::vec3 normal;
	};

	class Hitable {
	public:
		virtual ~Hitable() = default;

		virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
	};

	class Sphere : public Hitable {
	public:
		Sphere(){center = glm::vec3(0.0f, 0.0f, 0.0f); radius = 0.5f;}
		Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) {}
		bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
		glm::vec3 center{};
		float radius;
	};
	bool Sphere::hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const {
		glm::vec3 oc = ray.origin() - center;
		float a = glm::dot(ray.direction(), ray.direction());
		float b = 2.0f * glm::dot(oc, ray.B);
		float c = glm::dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;
		if (discriminant > 0.0f) {
			float temp{(-b - sqrt(discriminant)) / (2.0f * a)};
			if (temp < t_max && temp > t_min) {
				rec.t = temp;
				rec.p = ray.point_at_paramter(rec.t);
				rec.normal = (rec.p - center) / radius;
				return true;
			}
			temp = {(-b + sqrt(discriminant)) / (2.0f * a)};
			if (temp < t_max && temp > t_min) {
				rec.t = temp;
				rec.p = ray.point_at_paramter(rec.t);
				rec.normal = (rec.p - center) / radius;
				return true;
			}
		}
		return false;
	}


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
		int m_imageWidth{ 1280 };
		int m_imageHeight{ 720 };
		void GenerateRayTraceImage(); // Ray tracing function
		// Define sphere properties
		Sphere bigSphere;
		std::vector<Sphere> scene_spheres;
		// Define camera properties


		glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);;
		float viewportHeight = 2.0f;
		float viewportWidth {};
		float focalLength = 2.0f;
		glm::vec3 horizontal {};
		glm::vec3 vertical{};
		glm::vec3 lowerLeftCorner{};
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
		m_input.ObserveKey(GLFW_KEY_W);
		m_input.ObserveKey(GLFW_KEY_D);
		m_input.ObserveKey(GLFW_KEY_A);
		m_input.ObserveKey(GLFW_KEY_S);
		m_input.ObserveKey(GLFW_KEY_E);
		m_input.ObserveKey(GLFW_KEY_Q);

		m_input.ObserveKey(GLFW_KEY_LEFT_SHIFT);
		m_input.ObserveKey(GLFW_KEY_LEFT_ALT);

		bigSphere = Sphere(glm::vec3(-0.5f, -1.5f, -4.0f), 1.0f);

		// Calculate the image plane
		scene_spheres ={
			Sphere(glm::vec3(1.0f, 0.0f, -2.0f), 0.5f),
			Sphere(glm::vec3(-4.0f, 2.0f, -10.0f), 5.0f),
			bigSphere,
		};

		m_renderer.Initialize();
	}



	void Raytracer::GenerateRayTraceImage()
	{
	    m_rayTraceImage.resize(m_imageWidth * m_imageHeight * 4);  // RGBA

		viewportWidth = (float)m_imageWidth / m_imageHeight * viewportHeight;
		horizontal= glm::vec3(viewportWidth, 0.0f, 0.0f);
		vertical  = glm::vec3(0.0f, viewportHeight, 0.0f);

		lowerLeftCorner = m_camera.getPosition() - horizontal / 2.0f - vertical / 2.0f + glm::vec3(0.0f, 0.0f, -focalLength);

	    // Loop through each pixel
	    for (int y = 0; y < m_imageHeight; ++y) {
	        for (int x = 0; x < m_imageWidth; ++x) {
	            int index = (y * m_imageWidth + x) * 4;

	            // Compute ray direction
	            float u = float(x) / (m_imageWidth  - 1);
	            float v = float(y) / (m_imageHeight - 1);
	            glm::vec3 dir = glm::normalize(lowerLeftCorner + u*horizontal + v*vertical - m_camera.getPosition());
	            Ray ray(m_camera.getPosition(), dir);

	            // Track the closest intersection
	            bool hitAnything   = false;
	            float closestSoFar = std::numeric_limits<float>::infinity();
	            const Sphere* hitSphere = nullptr;
	            float hitT = 0.0f;

	            // Find nearest sphere intersection
	            for (auto& sphere : scene_spheres) {
	            	HitRecord hitRecord;

	                if (sphere.hit(ray, 0, closestSoFar, hitRecord)) {
	                    hitAnything   = true;
	                    closestSoFar  = hitRecord.t;
	                    hitSphere     = &sphere;
	                    hitT          = hitRecord.t;
	                }
	            }

	            if (hitAnything) {
	                // Compute normal at hit point
	                glm::vec3 hitPoint = ray.point_at_paramter(hitT);
	                glm::vec3 normal   = glm::normalize(hitPoint - hitSphere->center);
	                glm::vec3 col      = 0.5f * glm::vec3(normal.x + 1.0f,
	                                                    normal.y + 1.0f,
	                                                    normal.z + 1.0f);

	                m_rayTraceImage[index + 0] = uint8_t(col.r * 255);
	                m_rayTraceImage[index + 1] = uint8_t(col.g * 255);
	                m_rayTraceImage[index + 2] = uint8_t(col.b * 255);
	                m_rayTraceImage[index + 3] = 255;
	            }
	            else {
	                // No hit → draw background
	                glm::vec3 bg = glm::vec3(1.0f); // white
	                m_rayTraceImage[index + 0] = uint8_t(bg.r * 255);
	                m_rayTraceImage[index + 1] = uint8_t(bg.g * 255);
	                m_rayTraceImage[index + 2] = uint8_t(bg.b * 255);
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
		// Render the ray traced texture as a full-screen quad
		m_renderer.RenderRayTraceTexture();
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
							 m_input.IsKeyDown(GLFW_KEY_Q), m_input.IsKeyDown(GLFW_KEY_E),
							 rotateLeft, rotateRight,
							 zoomIn, zoomOut);

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
			ImGui::Text("Render Mode: %s", m_camera.DebugMode().c_str());

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


			ImGui::SliderFloat("X-Position", &scene_spheres.at(0).center.x, -10.0f, 10.0f);
			ImGui::SliderFloat("Y-Position", &scene_spheres.at(0).center.y, -6.0f, 6.0f);
			ImGui::SliderFloat("Z-Position", &scene_spheres.at(0).center.z, -25.0f, 5.0f);
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}



}
