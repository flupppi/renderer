module;
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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
#pragma region Helper Functions
	/**
	 * @enum RenderMode
	 * @brief Defines the rendering modes for rendering graphical elements.
	 *
	 * This enumeration is used to specify the rendering style or technique
	 * applied in graphical systems. The modes determine how visual content
	 * is processed and displayed on the screen.
	 */
	enum RenderMode {
		diffuse,
		normals,
	};

	/**
	 * @var renderModeNames
	 * @brief Stores the names corresponding to various rendering modes.
	 *
	 * This variable holds a collection of string representations or labels
	 * that are used to identify and differentiate various rendering modes.
	 * It acts as a mapping resource to associate rendering modes with their
	 * descriptive or user-friendly names for display or reference purposes.
	 */
	static const char* renderModeNames[] = { "Diffuse", "Normals" };

	/**
	 * @brief Generates a random floating-point number between 0 and 1.
	 *
	 * This function calculates and returns a pseudorandom number that falls
	 * within the range [0.0, 1.0). It is commonly used in simulations, procedural
	 * content generation, or any scenario requiring random decimal values.
	 *
	 * @return A random double value in the range [0.0, 1.0).
	 */
	export double rand01()
	{
		// 1) a thread‐local engine so calls from different functions/threads
		//    don’t race each other and performance is OK
		static thread_local std::mt19937_64 engine{ std::random_device{}() };

		// 2) use generate_canonical to get full precision in [0,1)
		return std::generate_canonical<double,
									   std::numeric_limits<double>::digits>(engine);
	}

	/**
	 * @brief Generates a random point inside a unit sphere.
	 *
	 * This method produces a random 3D point uniformly distributed within the
	 * boundaries of a sphere with a radius of 1, centered at the origin.
	 * It ensures that the generated point lies strictly within the sphere.
	 *
	 * @return A 3D vector representing a random point inside the unit sphere.
	 */
	glm::vec3 random_in_unit_sphere(){
		glm::vec3 p;
		do {
			p = 2.0*glm::vec3(rand01(), rand01(), rand01()) - glm::vec3(1.0f, 1.0f, 1.0f);
		}while (length2(p) >= 1.0);
		return p;
	}
#pragma endregion
#pragma region Material Definition
	/**
	 * @brief A raytracer simulates the interaction of the light with the environment.
	 * We need to define materials that the light can interact with.
	 */
	struct HitRecord;
	/**
	 * @class Material
	 * @brief The material base class represents the properties and behavior of a material in the rendering system.
	 *
	 * For the start we only need to define a method that scatters the light. Each material implement a different scatter
	 * calculation that defines it's appearance.
	 */
	class Material {
	public:
		virtual ~Material() = default;

		/**
		 * @brief the scatter function takes a ray that hits a surface and computes a scattered ray combined with additional data in the hit record.
		 */
		virtual bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const = 0;
	};
	/**
	 * @class HitRecord
	 * @brief Represents a record of a hit in ray tracing or collision detection.
	 *
	 * This class encapsulates the details of an interaction point including
	 * the position, normal direction at the hit, and other metadata to describe
	 * the event. We could also pass each of these individually to the methods, but since all of these will be required at most points it makes sense to keep them together.
	 */
	struct HitRecord {
		float t;
		glm::vec3 p;
		glm::vec3 normal;
		std::shared_ptr<Material> mat_ptr;
	};

	/**
	 * @class Hitable
	 * @brief Represents an abstract interface for objects that can be "hit" in a ray tracing system.
	 *
	 * The Hitable class defines a framework for objects that can participate
	 * in ray-object intersection calculations. It is important since our material can only be applied on a surface that is also hitable.
	 */
	class Hitable {
	public:
		virtual ~Hitable() = default;
		virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
	};



	/**
	 * @brief Now next we actually define the materials.
	 *
	 * Currently there are only three of them that we define.
	 * A lambertian material, metal and glass (dielectric). We will later add more of them and make them more complex but these three already show us a lot.
	 */

	/**
	 * @class Lambertian
	 * @brief Represents a Lambertian reflectance material model.
	 *
	 * This class models a diffuse surface where the reflected light
	 * is scattered uniformly in all directions. It is commonly used
	 * in rendering systems to simulate the behavior of matte or non-glossy surfaces.
	 * Objects using the Lambertian model reflect light based on the surface's
	 * albedo and the incident light properties.
	 */
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


	/**
	 * @class Metal
	 * @brief Represents the properties and behaviors of a metallic material.
	 *
	 * This class is designed to encapsulate characteristics, attributes, and
	 * actions associated with metal objects. It provides a framework to
	 * define and manipulate metallic materials in various systems or simulations.
	 */
	class Metal final : public Material {
	public:
		explicit Metal(const glm::vec3& a, float f): albedo(a){if (f<1.0f) fuzz = f; else fuzz = 1.0f;}
		bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override {
			glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
			scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
			attenuation = albedo;
			return (dot(scattered.direction(), rec.normal) > 0.0f);
		}
		glm::vec3 albedo;
		float fuzz;
	};

	/**
	 * @class Dielectric
	 * @brief Now the dielectric material is a bit more complex since it has different terms that we have to combine.
	 *
	 * A reflective term, a refractive term and for the refractive part we need this schlick function to approximate the fresnel term in an elegant fashion.
	 */
	class Dielectric;

	/**
	 * @brief Reflects a value or object based on specific conditions.
	 *
	 * This function is used to perform a reflection operation,
	 * typically reversing or mirroring a given input according
	 * to the defined logic.
	 *
	 * @param input The value or object to be reflected.
	 * @return The reflected value or object.
	 */
	export glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
		return v - 2.0f * glm::dot(v, n ) * n;
	}

	/**
	 * @brief Computes the Schlick approximation for Fresnel effect.
	 *
	 * The Schlick approximation is used to estimate the Fresnel reflectance
	 * of a surface based on the viewing angle and surface properties. This
	 * function provides an efficient way to calculate approximate reflectance
	 * for use in lighting and shading calculations.
	 *
	 * The approximation is often used in physically-based rendering pipelines
	 * to achieve realistic light reflections.
	 */
	export float schlick(float cosine, float ref_idx) {
		float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
		r0 = glm::pow2(r0);
		return r0 + (1.0f - r0) * glm::pow((1.0f - cosine), 5.0f);
	}

	/**
	 * @brief the refraction function calculates the refracted vector.
	 *
	 *
	 */
	export bool refract(const glm::vec3& v, const glm::vec3& n, float ni_over_nt, glm::vec3& refracted) {
		glm::vec3 uv = glm::normalize(v);
		float dt = glm::dot(uv, n);
		float discriminant = 1.0f - glm::pow2(ni_over_nt) * (1.0f - glm::pow2(dt));
		if (discriminant > 0.0f) {
			refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
			return true;
		}
		return false;
	}

	/**
	 * @class Dielectric
	 * @brief Represents a dielectric material with specific optical properties.
	 *
	 * The Dielectric class models materials that are non-conductive and have the
	 * ability to transmit electromagnetic waves, such as light, with minimal absorption.
	 * It is typically used in simulations or applications where optical interactions
	 * with transparent or semi-transparent materials need to be computed.
	 *
	 * This class usually defines properties such as refractive index or other attributes
	 * relevant to light-matter interactions.
	 */
	class Dielectric final : public Material {
	public:
		explicit Dielectric(float ri) : ref_idx(ri) {}
		bool scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override {
			glm::vec3 outward_normal;
			glm::vec3 reflected = reflect(r_in.direction(), rec.normal);
			float ni_over_nt;
			attenuation = glm::vec3(1.0f);
			glm::vec3 refracted;
			float reflect_prob;
			float cosine;
			if (glm::dot(r_in.direction(), rec.normal) > 0.0f) {
				outward_normal = -rec.normal;
				ni_over_nt = ref_idx;
				cosine = ref_idx * glm::dot(r_in.direction(), rec.normal) / length(r_in.direction());
			}else {
				outward_normal = rec.normal;
				ni_over_nt = 1.0f / ref_idx;
				cosine = -glm::dot(r_in.direction(), rec.normal) / length(r_in.direction());
			}
			if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
				reflect_prob = schlick(cosine, ref_idx);
			}else {
				scattered = Ray(rec.p, reflected);
				reflect_prob = 1.0f;
			}
			if (rand01() < reflect_prob) {
				scattered = Ray(rec.p, reflected);
			}else {
				scattered = Ray(rec.p, refracted);
			}
			return true;
		}
		float ref_idx;
	};
	/**
	 * @brief  Now that we defined all three materials we can apply it to a hitable object.
	 * In this case we use the sphere as a simple object. Later on we will need to define these material interactions also for triangles and other surface to make them generic material models for all kinds of light interactions.
	 */


	/**
	 * @class Sphere
	 * @brief Represents a 3D sphere in a geometric space.
	 *
	 * This class provides functionalities to define and manipulate a sphere
	 * in three-dimensional space. It manages properties such as radius and
	 * spatial position and offers methods for performing calculations related
	 * to the sphere, including volume, surface area, and intersection tests
	 * with other geometrical objects.
	 */
#pragma endregion
#pragma region Hitable Objects

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

#pragma endregion

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

#pragma region Program Setup


	//************************************
	// Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
	//************************************
	void Raytracer::ClearResources()
	{
		m_renderer.ClearResources();
	}

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
		auto metal = std::make_shared<Metal>(glm::vec3(0.5f, 0.8f, 0.8f), 0.3f);
		auto metal2 = std::make_shared<Metal>(glm::vec3(0.5f, 0.8f, 0.8f), 0.05f);
		auto metal3 = std::make_shared<Metal>(glm::vec3(0.5f, 0.8f, 0.8f), 0.7f);
		auto glass = std::make_shared<Dielectric>(1.5f);
		world.emplace<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), diffuse, 0.5f);
		world.emplace<Sphere>(glm::vec3(2.0f, 0.0f, -1.0f), metal2, 0.5f);
		world.emplace<Sphere>(glm::vec3(1.0f, 0.0f, -0.5f), glass, 0.3f);
		world.emplace<Sphere>(glm::vec3(-2.0f, 0.0f, -1.0f), metal3, 0.5f);
		world.emplace<Sphere>(glm::vec3(0.0f, -100.5f, -1.0f),metal, 100.0f);
		world.emplace<Sphere>(glm::vec3(1.0f, 0.0f, -10.0f), diffuse2, 5.0f);



		m_renderer.Initialize();
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
#pragma endregion
#pragma region Ray Tracing
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
#pragma endregion



}
