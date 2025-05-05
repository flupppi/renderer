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
#include <list>
#include <glm/detail/_noise.hpp>
#include <glm/detail/_noise.hpp>
#include <glm/detail/_noise.hpp>
#include <glm/detail/_noise.hpp>
#include <meta/meta.hpp>
#include <utility>
export module Raytracer;
import std;
import GameInterface;
import Camera;
import Quad;
import InputSystem;
import RaytracerRenderer;
namespace Engine {

	enum RenderMode {
		diffuse,
		normals,
	};
	static const char* renderModeNames[] = { "Diffuse", "Normals" };

	// Returns a random double in [0,1)
	inline double rand01()
	{
		// 1) a thread‐local engine so calls from different functions/threads
		//    don’t race each other and performance is OK
		static thread_local std::mt19937_64 engine{ std::random_device{}() };

		// 2) use generate_canonical to get full precision in [0,1)
		return std::generate_canonical<double,
									   std::numeric_limits<double>::digits>(engine);
	}
	glm::vec3 random_in_unit_sphere(){
		glm::vec3 p;
		do {
			p = 2.0*glm::vec3(rand01(), rand01(), rand01()) - glm::vec3(1.0f, 1.0f, 1.0f);
		}while (length2(p) >= 1.0);
		return p;
	}

	struct HitRecord;
	class Material {
	public:
		virtual ~Material() = default;

		virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const = 0;
	};

	struct HitRecord {
		float t;
		glm::vec3 p;
		glm::vec3 normal;
		std::shared_ptr<Material> mat_ptr;
	};






	class Hitable {
	public:
		virtual ~Hitable() = default;

		virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
	};

	class Lambertian final : public Material {
	public:
		explicit Lambertian(const glm::vec3& a): albedo(a) {}
		bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override {
			glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
			scattered = Ray(rec.p, target - rec.p);
			attenuation = albedo;
			return true;
		}
		glm::vec3 albedo;
	};

	glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
		return v - 2.0f * glm::dot(v, n ) * n;
	}



	class Metal final : public Material {
	public:
		explicit Metal(const glm::vec3& a): albedo(a){}
		bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override {
			glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
			scattered = Ray(rec.p, reflected);
			attenuation = albedo;
			return (dot(scattered.direction(), rec.normal) > 0.0f);
		}
		glm::vec3 albedo;
	};



	class Sphere final : public Hitable {
	public:
		Sphere(const glm::vec3& center, std::shared_ptr<Material> mat, float radius) : center(center), mat(std::move(mat)),radius(radius) {}
		bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
		glm::vec3 center{};
		float radius;
		std::shared_ptr<Material> mat;
	};
	bool Sphere::hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const {
		const glm::vec3 oc = ray.origin() - center;
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
				rec.mat_ptr = mat;
				return true;
			}
			temp = {(-b + sqrt(discriminant)) / (2.0f * a)};
			if (temp < t_max && temp > t_min) {
				rec.t = temp;
				rec.p = ray.point_at_paramter(rec.t);
				rec.normal = (rec.p - center) / radius;
				rec.mat_ptr = mat;
				return true;
			}
		}
		return false;
	}

	class HitableList: public Hitable {
	public:
		template<typename T, typename... Args>
		void emplace(Args&&... args) {
			objects_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
		}
		bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
	private:
		std::vector<std::unique_ptr<Hitable>> objects_;
	};

	bool HitableList::hit(const Ray &ray, float t_min, float t_max, HitRecord &rec) const {
		HitRecord temp_rec{};
		bool hit_anything = false;
		float closest_so_far = t_max;
		for (auto const& obj : objects_) {
			if (obj->hit(ray, t_min, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
		return hit_anything;
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
		glm::vec3 color(const Ray &r, int depth);
		HitableList world{};

		RenderMode m_renderMode = RenderMode::diffuse;
		int samples = 1;
		glm::vec3 colorModeNormal(const Ray& r);
		// Image buffer for ray tracing output
		std::vector<uint8_t> m_rayTraceImage;
		int m_imageWidth{ 1280 };
		int m_imageHeight{ 720 };
		void GenerateRayTraceImage(); // Ray tracing function
		// Define camera properties
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

		auto diffuse = std::make_shared<Lambertian>(glm::vec3(0.5f, 0.5f, 0.5f));
		auto diffuse2 = std::make_shared<Lambertian>(glm::vec3(0.2f, 0.5f, 0.2f));
		auto metal = std::make_shared<Metal>(glm::vec3(0.5f, 0.8f, 0.8f));
		world.emplace<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), diffuse, 0.5f);
		world.emplace<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f),metal, 100.0f);
		world.emplace<Sphere>(glm::vec3(1.0f, 0.0f, -10.0f), diffuse2, 5.0f);


		m_renderer.Initialize();
	}

	glm::vec3 Raytracer::color(const Ray& r, int depth) {
		HitRecord rec{};
		if (world.hit(r, 0.001f, std::numeric_limits<float>::infinity(), rec)) {
			Ray scattered{};
			glm::vec3 attenuation{};
			if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
				return attenuation * color(scattered, depth+1);
			}else {
				return {0.0f, 0.0f, 0.0f};
			}
		}else{
			glm::vec3 unit_direction = glm::normalize(r.direction());
			float t = 0.5f * (unit_direction.y + 1.0f);
			return (1.0f-t)*glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
		}
	}

	glm::vec3 Raytracer::colorModeNormal(const Ray& r) {
		HitRecord rec{};
		if (world.hit(r, 0.001f, std::numeric_limits<float>::infinity(), rec)) {
			return 0.5f * (rec.normal +glm::vec3(1.0f));
		}else{
			glm::vec3 unit_direction = glm::normalize(r.direction());
			float t = 0.5f * (unit_direction.y + 1.0f);
			return (1.0f-t)*glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
		}
	}

	void Raytracer::GenerateRayTraceImage()
	{
		m_rayTraceImage.resize(m_imageWidth * m_imageHeight * 4);  // RGBA

		// Precompute denominators as floats:
		float invW = 1.0f / float(m_imageWidth);
		float invH = 1.0f / float(m_imageHeight);

		for (int y = 0; y < m_imageHeight; ++y) {
			for (int x = 0; x < m_imageWidth; ++x) {
				glm::vec3 col(0.0f);
				// accumulate ns samples
				for (int s = 0; s < samples; ++s) {
					// jittered sample in [0,1)
					float u = (x + rand01()) * invW;
					float v = (y + rand01()) * invH;

					Ray ray = m_camera.getRay(u, v);
					switch (m_renderMode) {
						case normals:
							col += colorModeNormal(ray);
							break;
						case diffuse:
							col += color(ray, 0);
							break;
						default:
							col += color(ray, 0);
					}
				}
				// average & gamma-correct (gamma=2.0)
				col /= float(samples);
				col = glm::sqrt(col);

				int index = (y * m_imageWidth + x) * 4;
				m_rayTraceImage[index + 0] = uint8_t(glm::clamp(col.r, 0.0f, 1.0f) * 255);
				m_rayTraceImage[index + 1] = uint8_t(glm::clamp(col.g, 0.0f, 1.0f) * 255);
				m_rayTraceImage[index + 2] = uint8_t(glm::clamp(col.b, 0.0f, 1.0f) * 255);
				m_rayTraceImage[index + 3] = 255;
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
		// Render the ray-traced texture as a full-screen quad
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
			ImGui::SliderInt("Samples per Pixel", &samples, 1, 8);

			int mode = static_cast<int>(m_renderMode);
			if (ImGui::Combo("Render Mode", &mode, renderModeNames, IM_ARRAYSIZE(renderModeNames))) {
				m_renderMode = static_cast<RenderMode>(mode);
			}
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}



}
