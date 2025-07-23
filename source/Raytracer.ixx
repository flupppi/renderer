module;
#include <GL/glew.h>
#include "vendor/stb_image/stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <meta/meta.hpp>
#include <utility>
#include <omp.h>
#include <yaml-cpp/yaml.h>

#include "stb_image_write.h"

export module Raytracer;
import std;
import GameInterface;
import Camera;
import Quad;
import InputSystem;
import Geometry;
import Ray;
import ShaderUtil;

using namespace std;

namespace Engine {
    class Material;

#pragma region Light
    class Light {
        public:
        glm::vec3 direction;
        glm::vec3 intensity;
    };
#pragma endregion
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
        aabb_debug,
        blinn_phong,
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
    static const char *renderModeNames[] = {"Diffuse", "Normals", "AABB Debug", "Blinn-Phong Shading"};

    /**
     * @brief Generates a random floating-point number between 0 and 1.
     *
     * This function calculates and returns a pseudorandom number that falls
     * within the range [0.0, 1.0). It is commonly used in simulations, procedural
     * content generation, or any scenario requiring random decimal values.
     *
     * @return A random double value in the range [0.0, 1.0).
     */
    export double rand01() {
        // 1) a thread‐local engine so calls from different functions/threads
        //    don’t race each other and performance is OK
        static thread_local mt19937_64 engine{uint64_t(1337 + omp_get_thread_num())};

        // 2) use generate_canonical to get full precision in [0,1)
        return generate_canonical<double, numeric_limits<double>::digits>(engine);
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
    glm::vec3 random_in_unit_sphere() {
        glm::vec3 p;
        do {
            p = 2.0 * glm::vec3(rand01(), rand01(), rand01()) - glm::vec3(1.0f, 1.0f, 1.0f);
        } while (length2(p) >= 1.0);
        return p;
    }

#pragma endregion
#pragma region Acceleration Structures


#pragma endregion
#pragma region Materials
    /**
 * @brief A raytracer simulates the interaction of the light with the environment.
 * We need to define materials that the light can interact with.
 */
    struct HitRecord;
    class TextureBase {
    public:
        virtual ~TextureBase() = default;

        virtual glm::vec3 value(float u, float v, const glm::vec3& p) const = 0;
    };
    class SolidColor : public TextureBase {
    public:
        SolidColor(const glm::vec3& albedo) : albedo(albedo) {}
        SolidColor(float red, float green, float blue) : SolidColor(glm::vec3(red,green,blue)) {}

        glm::vec3 value(float u, float v, const glm::vec3& p) const override {
            return albedo;
        }

    private:
        glm::vec3 albedo;
    };

    class CheckerTexture : public TextureBase {

    public:
        CheckerTexture(float scale, shared_ptr<TextureBase> even, shared_ptr<TextureBase> odd)
          : inv_scale(1.0 / scale), even(even), odd(odd) {}

        CheckerTexture(float scale, const glm::vec3& c1, const glm::vec3& c2)
          : CheckerTexture(scale, make_shared<SolidColor>(c1), make_shared<SolidColor>(c2)) {}

        glm::vec3 value(float u, float v, const glm::vec3& p) const override {
            auto xInteger = int(std::floor(inv_scale * p.x));
            auto yInteger = int(std::floor(inv_scale * p.y));
            auto zInteger = int(std::floor(inv_scale * p.z));

            bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

            return isEven ? even->value(u, v, p) : odd->value(u, v, p);
        }

    private:
        double inv_scale;
        shared_ptr<TextureBase> even;
        shared_ptr<TextureBase> odd;
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
        float u;
        float v;
        glm::vec3 p;
        glm::vec3 normal;
        shared_ptr<Material> mat_ptr;
        bool front_face;
        void set_face_normal(const Ray& r, const glm::vec3& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
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

        virtual bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const = 0;
        virtual bool boundingBox(AABB &box) const = 0;
    };

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
        virtual bool scatter(const Ray &r_in, const HitRecord &rec, glm::vec3 &attenuation, Ray &scattered) const = 0;
        virtual glm::vec3 emitted() const {
            return glm::vec3{0,0,0};
        }
    };

    class DiffuseLight : public Material {
    public:
        DiffuseLight(const glm::vec3& emit) : color(emit) {}

        bool scatter(const Ray &r_in, const HitRecord &rec, glm::vec3 &attenuation, Ray &scattered) const override{
            return false;
        }

        glm::vec3 emitted() const override {
            return color;
        }

    private:
        glm::vec3 color;
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

        explicit Lambertian(const glm::vec3 &a): tex(make_shared<SolidColor>(a)) {}
        explicit Lambertian(shared_ptr<TextureBase> tex) : tex(tex) {}

        bool scatter(const Ray &r_in, const HitRecord &rec, glm::vec3 &attenuation, Ray &scattered) const override {
            glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
            scattered = Ray(rec.p, target - rec.p);
            attenuation = tex->value(rec.u, rec.v, rec.p);
            return true;
        }

        shared_ptr<TextureBase> tex;
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
        explicit Metal(const glm::vec3 &a, float f): albedo(a) {
            if (f < 1.0f) fuzz = f;
            else fuzz = 1.0f;
        }

        bool scatter(const Ray &r_in, const HitRecord &rec, glm::vec3 &attenuation, Ray &scattered) const override {
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
    export glm::vec3 reflect(const glm::vec3 &v, const glm::vec3 &n) {
        return v - 2.0f * glm::dot(v, n) * n;
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
    export bool refract(const glm::vec3 &v, const glm::vec3 &n, float ni_over_nt, glm::vec3 &refracted) {
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
        explicit Dielectric(float ri) : ref_idx(ri) {
        }

        bool scatter(const Ray &r_in, const HitRecord &rec, glm::vec3 &attenuation, Ray &scattered) const override {
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
            } else {
                outward_normal = rec.normal;
                ni_over_nt = 1.0f / ref_idx;
                cosine = -glm::dot(r_in.direction(), rec.normal) / length(r_in.direction());
            }
            if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
                reflect_prob = schlick(cosine, ref_idx);
            } else {
                scattered = Ray(rec.p, reflected);
                reflect_prob = 1.0f;
            }
            if (rand01() < reflect_prob) {
                scattered = Ray(rec.p, reflected);
            } else {
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
    class BlinnPhongMaterial final : public Material {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        BlinnPhongMaterial(glm::vec3 a, glm::vec3 d, glm::vec3 s, float sh)
            : ambient(a), diffuse(d), specular(s), shininess(sh) {}

        bool scatter(const Ray &r_in, const HitRecord &rec, glm::vec3 &attenuation, Ray &scattered) const override {
            return false;  // skip recursion
        }
        glm::vec3 shade(const Ray& r_in, const HitRecord& rec, const std::vector<Light>& lights, const Hitable& world) const {
            glm::vec3 viewDir = glm::normalize(-r_in.direction());
            glm::vec3 result = ambient;

            for (const auto& light : lights) {
                glm::vec3 lightDir = glm::normalize(-light.direction);
                glm::vec3 h = glm::normalize(viewDir + lightDir);

                // Shadow ray
                glm::vec3 shadowOrigin = rec.p + 0.001f * rec.normal;
                Ray shadowRay{shadowOrigin, lightDir};

                HitRecord shadowHit;
                Interval shadowRange{0.001f, std::numeric_limits<float>::infinity()};
                bool inShadow = world.hit(shadowRay, shadowRange, shadowHit);

                if (!inShadow) {
                    float diff = std::max(glm::dot(rec.normal, lightDir), 0.0f);
                    float spec = std::pow(std::max(glm::dot(rec.normal, h), 0.0f), shininess);

                    glm::vec3 diffuseTerm = diffuse * diff;
                    glm::vec3 specularTerm = specular * spec;

                    result += light.intensity * (diffuseTerm + specularTerm);
                }
            }

            return result;
        }

    };


#pragma endregion
#pragma region Hitable Objects
    class DebugAABB : public Hitable {
    public:
        explicit DebugAABB(const AABB& box) : box(box) {}

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
            // Ray-AABB intersection returns true if we hit the box
            if (slabsBoxTest(box.t0, box.t1, r.origin(), 1.0f / r.direction(), ray_t.min, ray_t.max)) {
                rec.t = ray_t.min;
                rec.p = r.point_at_paramter(rec.t);
                rec.normal = glm::vec3(1, 0, 0);  // dummy normal
                rec.mat_ptr = std::make_shared<Lambertian>(glm::vec3(1, 0, 0));
                return true;
            }
            return false;
        }

        bool boundingBox(AABB& out_box) const override {
            out_box = box;
            return true;
        }

    private:
        AABB box;
    };

    class Triangle : public Hitable {
    public:
        Triangle(const glm::vec3& v0,
            const glm::vec3& v1,
            const glm::vec3& v2,
            std::shared_ptr<Material> mat):
            v0(v0), v1(v1),v2(v2),mat(std::move(mat)){}
        bool hit(const Ray &ray, Interval ray_t, HitRecord &rec) const override;
        bool boundingBox(AABB& box) const override {
            glm::vec3 min = glm::min(glm::min(v0, v1), v2);
            glm::vec3 max = glm::max(glm::max(v0, v1), v2);
            box = AABB(min, max);
            return true;
        }

    private:
        glm::vec3 v0, v1, v2;
        std::shared_ptr<Material> mat;

    };
    bool Triangle::hit(const Ray &ray, Interval ray_t, HitRecord &rec) const {
        // RAY - TRIANGLE Intersection with Möller-Trumbore Algorithm: https://doi.org/10.1080/10867651.1997.10487468
        const float EPSILON = 1e-6f;
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;


        glm::vec3 pvec = glm::cross(ray.direction(), edge2);
        float det = glm::dot(edge1, pvec);

        if (std::abs(det) < EPSILON) return false;

        float inv_det = 1.0f / det;
        glm::vec3 tvec = ray.origin() - v0;

        float u = glm::dot(tvec, pvec) * inv_det;
        if (u < 0.0f || u > 1.0f) return false;

        glm::vec3 qvec = glm::cross(tvec, edge1);
        float v = glm::dot(ray.direction(), qvec) * inv_det;
        if (v < 0.0f || u + v > 1.0f) return false;

        float t = glm::dot(edge2, qvec) * inv_det;
        if (!ray_t.contains(t)) return false;

        rec.t = t;
        rec.p = ray.point_at_paramter(t);
        rec.normal = glm::normalize(glm::cross(edge1, edge2));
        rec.mat_ptr = mat;

        return true;
    }

    class Quadrilateral final : public Hitable {
    public:
        Quadrilateral(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, std::shared_ptr<Material> mat) {
            tri1 = std::make_unique<Triangle>(a, b, c, mat);
            tri2 = std::make_unique<Triangle>(a, c, d, mat);
        }

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
            HitRecord temp;
            bool hit1 = tri1->hit(r, ray_t, temp);
            bool hit2 = tri2->hit(r, Interval(ray_t.min, hit1 ? temp.t : ray_t.max), temp);
            if (hit1 || hit2) {
                rec = temp;
                return true;
            }
            return false;
        }
        bool boundingBox(AABB& box) const override {
            AABB box1, box2;
            if (!tri1->boundingBox(box1) || !tri2->boundingBox(box2))
                return false;
            box = AABB(glm::min(box1.t0, box2.t0), glm::max(box1.t1, box2.t1));
            return true;
        }


    private:
        std::unique_ptr<Triangle> tri1;
        std::unique_ptr<Triangle> tri2;
    };

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

    class Sphere final : public Hitable {
    public:
        Sphere(const glm::vec3 &center, shared_ptr<Material> mat, float radius) : center(center), mat(std::move(mat)),
            radius(radius) {
        }
        bool hit(const Ray &ray, Interval ray_t, HitRecord &rec) const override {
            glm::vec3 oc = ray.origin() - center;
            float a = glm::dot(ray.direction(), ray.direction());
            float h = glm::dot(ray.direction(), oc);
            float c = glm::dot(oc, oc) - radius * radius;
            float discriminant = h * h - a * c;

            if (discriminant < 0) return false;

            float sqrtd = std::sqrt(discriminant);

            // First root
            float root = (-h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (-h + sqrtd) / a;
                if (!ray_t.surrounds(root))
                    return false;
            }

            rec.t = root;
            rec.p = ray.point_at_paramter(root);
            glm::vec3 normal = (rec.p - center) / radius;
            rec.set_face_normal(ray, normal );
            get_sphere_uv(normal, rec.u, rec.v);
            rec.mat_ptr = mat;
            return true;
        }
        static void get_sphere_uv(const glm::vec3& p, float& u, float& v) {
            auto theta = std::acos(-p.y);
            auto phi = std::atan2(-p.z, p.x) + glm::pi<float>();

            u = phi / (2*glm::pi<float>());
            v = theta / glm::pi<float>();
        }

        bool boundingBox(AABB& box) const override {
            glm::vec3 offset(radius, radius, radius);
            box = AABB(center - offset, center + offset);
            return true;
        }


        glm::vec3 center{};
        float radius;
        shared_ptr<Material> mat;
    };



    class HitableList : public Hitable {
    public:
        template<typename T, typename... Args>
        void emplace(Args &&... args) {
            objects_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        bool hit(const Ray &ray, Interval ray_t, HitRecord &rec) const override {
            HitRecord temp_rec{};
            bool hit_anything = false;
            float closest_so_far = ray_t.max;
            for (auto const &obj: objects_) {
                //AABB box;

                //if (obj->boundingBox(box) && box.hit(ray, t_min, closest_so_far)) {
                    if (obj->hit(ray, Interval(ray_t.min, closest_so_far), temp_rec)) {
                        hit_anything = true;
                        closest_so_far = temp_rec.t;
                        rec = temp_rec;
                 //   }
                }

            }
            return hit_anything;
        }

        bool boundingBox(AABB& box) const override {
            if (objects_.empty()) return false;

            AABB tempBox;
            bool first = true;
            for (const auto& obj : objects_) {
                if (!obj->boundingBox(tempBox))
                    return false;
                box = first ? tempBox : AABB(glm::min(box.t0, tempBox.t0), glm::max(box.t1, tempBox.t1));
                first = false;
            }
            return true;
        }
        const std::vector<std::unique_ptr<Hitable>>& getObjects() const { return objects_; }
    private:
        vector<unique_ptr<Hitable> > objects_;
    };



    class UVSphere final : public Hitable {
    public:
        explicit UVSphere(int n_slices, int n_stacks, shared_ptr<Material> mat)
        {
            std::vector<glm::vec3> vertices;
            // add top vertex
            auto v0 = glm::vec3(0.0f, 1.0f, 0.0f);
            vertices.emplace_back(v0);

            // generate vertices per stack / slice
            for (int i = 0; i < n_stacks -1; i++) {
                auto phi = M_PI * double(i + 1) / double(n_stacks);
                for (int j = 0; j < n_slices; j++) {
                    auto theta = 2.0f * M_PI * double(j) / double(n_slices);
                    auto x = std::sin(phi) * std::cos(theta);
                    auto y = std::cos(phi);
                    auto z = std::sin(phi) * std::sin(theta);
                    vertices.emplace_back(glm::vec3(x, y, z));
                }
            }

            // add bottom vertex
            auto v1 = vertices.emplace_back(glm::vec3(0, -1, 0));

            // add top / bottom triangles
            for (int i = 0; i < n_slices; ++i)
            {
                auto i0 = i + 1;
                auto i1 = (i + 1) % n_slices + 1;
                surfaceMesh.emplace<Triangle>(v0, vertices[i1], vertices[i0], mat);
                surfaceMesh.emplace<Triangle>(v0, vertices[i1], vertices[i0], mat);
                i0 = i + n_slices * (n_stacks - 2) + 1;
                i1 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;
                surfaceMesh.emplace<Triangle>(v1, vertices[i0], vertices[i1], mat);
            }

            // add quads per stack / slice
            for (int j = 0; j < n_stacks - 2; j++)
            {
                auto j0 = j * n_slices + 1;
                auto j1 = (j + 1) * n_slices + 1;
                for (int i = 0; i < n_slices; i++)
                {
                    auto i0 = j0 + i;
                    auto i1 = j0 + (i + 1) % n_slices;
                    auto i2 = j1 + (i + 1) % n_slices;
                    auto i3 = j1 + i;
                    surfaceMesh.emplace<Quadrilateral>(vertices[i0], vertices[i1],
                                  vertices[i2], vertices[i3], mat);
                }
            }
        }
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
            return surfaceMesh.hit(r, ray_t, rec);
        }
        bool boundingBox(AABB& box) const override {
            return surfaceMesh.boundingBox(box);
        }

    private:
        HitableList surfaceMesh;


    };

    class Cone final : public Hitable {
    public:
        explicit Cone(int resolution, float radius, float height, shared_ptr<Material> mat)
        {
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec3> base_vertices;
            // add vertices subdividing a circle
            for (int i = 0; i < resolution; i++) {
                float ratio = static_cast<float>(i) / (resolution);
                float r = ratio * (M_PI * 2.0);
                float x = std::cos(r) * radius;
                float z = std::sin(r) * radius;
                auto v = vertices.emplace_back(glm::vec3(x, 0.0, z));
                base_vertices.emplace_back(v);
            }

            // add the tip of the cone
            auto v0 = vertices.emplace_back(glm::vec3(0.0, height, 0.0));

            // generate triangular faces
            for (int i = 0; i < resolution; i++) {
                auto ii = (i + 1) % resolution;
                surfaceMesh.emplace<Triangle>(v0, vertices[ii], vertices[i], mat);
            }

            // reverse order for consistent face orientation
            std::reverse(base_vertices.begin(), base_vertices.end());

            glm::vec3 base_center(0.0f, 0.0f, 0.0f);
            for (int i = 0; i < resolution; ++i) {
                int j = (i + 1) % resolution;
                surfaceMesh.emplace<Triangle>(base_center, base_vertices[j], base_vertices[i], mat);
            }


        }
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
            return surfaceMesh.hit(r, ray_t, rec);
        }
        bool boundingBox(AABB& box) const override {
            return surfaceMesh.boundingBox(box);
        }
    private:
        HitableList surfaceMesh;


    };

    class Cube final : public Hitable {
    public:
        explicit Cube(std::shared_ptr<Material> mat) {
            addFace({ 0, 0, 1}, mat);  // front
            addFace({ 0, 0,-1}, mat);  // back
            addFace({ 0, 1, 0}, mat);  // top
            addFace({ 0,-1, 0}, mat);  // bottom
            addFace({ 1, 0, 0}, mat);  // right
            addFace({-1, 0, 0}, mat);  // left

        }

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
            return faces.hit(r, ray_t, rec);
        }
        bool boundingBox(AABB& box) const override {
            return faces.boundingBox(box);
        }

    private:
        HitableList faces;

        void addFace(const glm::vec3& normal, std::shared_ptr<Material> mat) {
            // Right-handed system: build a quad with normal and two tangent vectors
            glm::vec3 up = (std::abs(normal.y) > 0.99f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
            glm::vec3 tangent = glm::normalize(glm::cross(normal, up));
            glm::vec3 bitangent = glm::normalize(glm::cross(tangent, normal));

            glm::vec3 center = 0.5f * normal;

            glm::vec3 p1 = center + 0.5f * (-tangent - bitangent);
            glm::vec3 p2 = center + 0.5f * ( tangent - bitangent);
            glm::vec3 p3 = center + 0.5f * ( tangent + bitangent);
            glm::vec3 p4 = center + 0.5f * (-tangent + bitangent);

            faces.emplace<Triangle>(p3, p2, p1, mat);
            faces.emplace<Triangle>(p4, p3, p1, mat);
        }
    };

    class Transform : public Hitable {
    public:
        Transform(std::unique_ptr<Hitable> object, glm::mat4 transform)
            : m_object(std::move(object)), m_transform(transform) {
            m_inverse = glm::inverse(transform);
            m_inverse_transpose = glm::transpose(m_inverse);
        }

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
            // Transform ray into object space
            glm::vec4 o = m_inverse * glm::vec4(r.origin(), 1.0f);
            glm::vec4 d = m_inverse * glm::vec4(r.direction(), 0.0f);
            Ray transformed_ray{glm::vec3(o), glm::vec3(d)};

            if (!m_object->hit(transformed_ray, ray_t, rec)) return false;

            // Transform hit point and normal back to world space
            rec.p = glm::vec3(m_transform * glm::vec4(rec.p, 1.0f));
            rec.normal = glm::normalize(glm::vec3(m_inverse_transpose * glm::vec4(rec.normal, 0.0f)));
            return true;
        }
        bool boundingBox(AABB& box) const override {
            AABB childBox;
            if (!m_object->boundingBox(childBox)) return false;

            glm::vec3 t0 = childBox.t0;
            glm::vec3 t1 = childBox.t1;

            glm::vec3 corners[8] = {
                {t0.x, t0.y, t0.z}, {t1.x, t0.y, t0.z}, {t0.x, t1.y, t0.z}, {t1.x, t1.y, t0.z},
                {t0.x, t0.y, t1.z}, {t1.x, t0.y, t1.z}, {t0.x, t1.y, t1.z}, {t1.x, t1.y, t1.z},
            };

            glm::vec3 minPt(FLT_MAX), maxPt(-FLT_MAX);
            for (auto& corner : corners) {
                glm::vec3 transformed = glm::vec3(m_transform * glm::vec4(corner, 1.0f));
                minPt = glm::min(minPt, transformed);
                maxPt = glm::max(maxPt, transformed);
            }

            box = AABB(minPt, maxPt);
            return true;
        }


    private:
        std::unique_ptr<Hitable> m_object;
        glm::mat4 m_transform;
        glm::mat4 m_inverse;
        glm::mat4 m_inverse_transpose;
    };

#pragma endregion
#pragma region Raytracer Renderer
	export class RaytracerRenderer
	{

	public:
		void Initialize(int width, int height);
		void InitializeFullScreenQuad();
		void RenderRayTraceTexture();
		void UpdateTexture(const std::vector<uint8_t>& image, int width, int height);  // New
		void UpdateTextureRow(std::span<const uint8_t> rowData, int x, int y, int width, int height) const
		{
			glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rowData.data());
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		void InitializeAABBRenderer() {
			glGenVertexArrays(1, &m_bboxVAO);
			glGenBuffers(1, &m_bboxVBO);

			glBindVertexArray(m_bboxVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_bboxVBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);
		}

		void RenderBoundingBox(const AABB& box, glm::mat4 mvp) const {
			glm::vec3 min = box.t0;
			glm::vec3 max = box.t1;

			glm::vec3 corners[] = {
				{min.x, min.y, min.z}, {max.x, min.y, min.z},
				{max.x, max.y, min.z}, {min.x, max.y, min.z},
				{min.x, min.y, max.z}, {max.x, min.y, max.z},
				{max.x, max.y, max.z}, {min.x, max.y, max.z}
			};

			GLuint indices[] = {
				0, 1, 1, 2, 2, 3, 3, 0,
				4, 5, 5, 6, 6, 7, 7, 4,
				0, 4, 1, 5, 2, 6, 3, 7
			};
			glLineWidth(2);
			glm::vec3 lineVertices[24];
			for (int i = 0; i < 24; ++i)
				lineVertices[i] = corners[indices[i]];
			glBindVertexArray(m_bboxVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_bboxVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glUseProgram(m_bbShader);
			glUniformMatrix4fv(glGetUniformLocation(m_bbShader, "transformation"), 1, GL_FALSE, glm::value_ptr(mvp));
			glDrawArrays(GL_LINES, 0, 24);
			glBindVertexArray(0);
		}

		void ClearResources();


	private:
	    int m_width=800, m_height=600;
		void LoadShaders();
		GLuint m_rayTraceTexture{ 0 };  // Texture ID for the ray-traced image
		GLuint m_fullScreenShader{ 0 };
		GLuint m_bbShader{ 0 };
		GLuint m_fullScreenVAO{ 0 };
		GLuint m_fullScreenVBO{ 0 };
		GLuint m_bboxVAO = 0;
		GLuint m_bboxVBO = 0;


	};
//************************************
	// Load and Initialize all Index and Vertex buffer Objects, Shaders, Vertex Array Objects and Textures that are needed for rendering the Model.
	//************************************
	void RaytracerRenderer::Initialize(int width, int height)
	{
	    m_width = width;
	    m_height = height;
		LoadShaders();
		// Unbind Buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// Initialize ray trace texture
		glGenTextures(1, &m_rayTraceTexture);
		glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width,m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);  // Placeholder size
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		InitializeFullScreenQuad();
		InitializeAABBRenderer();
	}
	void RaytracerRenderer::InitializeFullScreenQuad()
	{
		// Define vertices for a full-screen quad with position and texture coordinates
		float quadVertices[] = {
			// Positions   // TexCoords
			-1.0f,  1.0f,   0.0f, 1.0f,  // Top-left
			-1.0f, -1.0f,   0.0f, 0.0f,  // Bottom-left
			 1.0f, -1.0f,   1.0f, 0.0f,  // Bottom-right

			-1.0f,  1.0f,   0.0f, 1.0f,  // Top-left
			 1.0f, -1.0f,   1.0f, 0.0f,  // Bottom-right
			 1.0f,  1.0f,   1.0f, 1.0f   // Top-right
		};

		// Generate and bind VAO/VBO
		glGenVertexArrays(1, &m_fullScreenVAO);
		glGenBuffers(1, &m_fullScreenVBO);

		glBindVertexArray(m_fullScreenVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_fullScreenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Unbind VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void RaytracerRenderer::UpdateTexture(const std::vector<uint8_t>& image, int width, int height)
	{
	    glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);

	    if (m_width != width || m_height != height) {
	        m_width = width;
	        m_height = height;
	        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width,m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);  // Placeholder size

	    }

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void RaytracerRenderer::RenderRayTraceTexture()
	{

		glUseProgram(m_fullScreenShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);
		glUniform1i(glGetUniformLocation(m_fullScreenShader, "rayTraceTexture"), 0);


		glBindVertexArray(m_fullScreenVAO);  // Full-screen quad VAO
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
	//************************************
	// Delete Buffers, VAOs and Shaders
	//************************************
	void RaytracerRenderer::ClearResources()
	{
		// Cleanup full-screen quad resources
		glDeleteVertexArrays(1, &m_fullScreenVAO);
		glDeleteBuffers(1, &m_fullScreenVBO);
		glDeleteProgram(m_fullScreenShader);
	}
	//************************************
	// Load the shader programs from their glsl files using the ShaderUtil.
	//************************************
	void RaytracerRenderer::LoadShaders()
	{
		// Load a full-screen shader program
		m_fullScreenShader = ShaderUtil::CreateShaderProgram("shaders/VFullScreenQuad.glsl", "shaders/FFullScreenQuad.glsl", nullptr);
		m_bbShader =  ShaderUtil::CreateShaderProgram("shaders/VJoint.glsl", "shaders/FJoint.glsl", nullptr);

	}

#pragma endregion
#pragma region Scene Loading
    std::shared_ptr<Engine::TextureBase> parseTexture(const YAML::Node& node) {
	    using namespace Engine;
	    if (node.IsSequence() && node.size() == 3) {
	        // Treat as solid color
	        return std::make_shared<SolidColor>(glm::vec3(node[0].as<float>(), node[1].as<float>(), node[2].as<float>()));
	    }

	    if (node["checker"]) {
	        const auto& checker = node["checker"];
	        float scale = checker["scale"] ? checker["scale"].as<float>() : 1.0f;

	        // Recursively parse even and odd nodes
	        auto even = parseTexture(checker["even"]);
	        auto odd = parseTexture(checker["odd"]);
	        return std::make_shared<CheckerTexture>(scale, even, odd);
	    }

	    std::cerr << "⚠️ Unknown texture format\n";
	    return std::make_shared<SolidColor>(glm::vec3(1.0f, 0.0f, 1.0f)); // magenta warning
	}

    void LoadSceneFromYaml(const std::filesystem::path &path, HitableList &world, Camera &camera) {
        YAML::Node scene = YAML::LoadFile(path.string());
        std::println("🏞️ Loading Scene from File {}", path.string());

        // Check if file can be parsed
        if (!scene["materials"] || !scene["objects"]) {
            std::cerr << "⚠️  Invalid scene file: must contain 'materials' and 'objects'\n";
            return;
        }



        // --- Load camera if present ---
        if (scene["camera"]) {
            const auto &camNode = scene["camera"];
            if (camNode["mode"]) {
                std::string modeStr = camNode["mode"].as<std::string>();
                if (modeStr == "FPS") camera.ToggleMode(); // defaults to Orbit, so toggle if needed
            }
            if (camNode["position"]) {
                auto pos = camNode["position"].as<std::vector<float> >();
                camera = Camera(); // reset to default constructor first
                camera.SetSpeed(camNode["speed"] ? camNode["speed"].as<float>() : 2.0f);
                camera.Move(0, false, false, false, false, false, false); // ensure up-to-date vectors
                camera.UpdateDirection(0, 0); // recalculate directions from yaw/pitch

                if (pos.size() == 3) {
                    camera.Move(0, false, false, false, false, false, false); // update internal state
                    camera.setPosition(glm::vec3(pos[0], pos[1], pos[2]));
                }
            }
            if (camNode["yaw"] || camNode["pitch"]) {
                float yaw = camNode["yaw"] ? camNode["yaw"].as<float>() : -90.0f;
                float pitch = camNode["pitch"] ? camNode["pitch"].as<float>() : 0.0f;
                camera.SetYawPitch(yaw, pitch);
            }

            if (camNode["fov"]) camera.setFOV(camNode["fov"].as<float>());
            if (camNode["orbit_distance"]) camera.SetDistance(camNode["orbit_distance"].as<float>());
            if (camNode["orbit_angle"]) camera.Update(0, false, false, false, false); // apply angle logic
            std::println("📷  Camera initialized from scene file");
        }

        std::unordered_map<std::string, std::shared_ptr<Material> > materials;

        // --- Load Materials ---
        for (const auto &matNode: scene["materials"]) {
            auto name = matNode["name"].as<std::string>();
            auto type = matNode["type"].as<std::string>();

            if (type == "diffuse") {
                if (matNode["texture"]) {
                    auto tex = parseTexture(matNode["texture"]);
                    materials[name] = std::make_shared<Lambertian>(tex);
                } else if (matNode["albedo"]) {
                    auto albedo = matNode["albedo"].as<std::vector<float> >();
                    materials[name] = std::make_shared<Lambertian>(
                        glm::vec3(albedo[0], albedo[1], albedo[2]));
                } else {
                    std::cerr << "⚠️ Diffuse material '" << name << "' missing 'albedo' or 'texture'\n";
                }

            } else if (type == "metal") {
                auto albedo = matNode["albedo"].as<std::vector<float> >();
                auto fuzz = matNode["fuzz"].as<float>();
                materials[name] = std::make_shared<Metal>(
                    glm::vec3(albedo[0], albedo[1], albedo[2]), fuzz);
            } else if (type == "glass") {
                auto ref_idx = matNode["ref_idx"].as<float>();
                materials[name] = std::make_shared<Dielectric>(ref_idx);
            } else if (type == "blinn_phong") {
                auto ambient = glm::vec3(matNode["ambient"][0].as<float>(), matNode["ambient"][1].as<float>(), matNode["ambient"][2].as<float>());
                auto diffuse = glm::vec3(matNode["diffuse"][0].as<float>(), matNode["diffuse"][1].as<float>(), matNode["diffuse"][2].as<float>());
                auto specular = glm::vec3(matNode["specular"][0].as<float>(), matNode["specular"][1].as<float>(), matNode["specular"][2].as<float>());
                auto shininess = matNode["shininess"].as<float>();
                materials[name] = std::make_shared<BlinnPhongMaterial>(ambient, diffuse, specular, shininess);
            }else if (type == "emissive") {
                auto emit = matNode["emit"].as<std::vector<float>>();
                if (emit.size() != 3) {
                    std::cerr << "⚠️  Emissive material '" << name << "' must define a 3-component 'emit' vector\n";
                    continue;
                }
                materials[name] = std::make_shared<DiffuseLight>(glm::vec3(emit[0], emit[1], emit[2]));
            }


            else {
                std::cerr << "⚠️  Unknown material type: " << type << "\n";
            }
        }


        for (const auto &obj: scene["objects"]) {
            // For every object a name, type, material, position and other type specific properties are defined.
            auto name = obj["name"].as<std::string>();
            auto type = obj["type"].as<std::string>();
            auto materialName = obj["material"].as<std::string>();
            glm::mat4 transform = glm::mat4(1.0f);

            if (obj["transform"]) {
                auto tf = obj["transform"];
                if (tf["translate"]) {
                    auto t = tf["translate"].as<std::vector<float>>();
                    transform = glm::translate(transform, glm::vec3(t[0], t[1], t[2]));
                }
                if (tf["rotate"]) {
                    auto r = tf["rotate"].as<std::vector<float>>();
                    transform = glm::rotate(transform, glm::radians(r[0]), glm::vec3(1, 0, 0));
                    transform = glm::rotate(transform, glm::radians(r[1]), glm::vec3(0, 1, 0));
                    transform = glm::rotate(transform, glm::radians(r[2]), glm::vec3(0, 0, 1));
                }
                if (tf["scale"]) {
                    auto s = tf["scale"].as<std::vector<float>>();
                    transform = glm::scale(transform, glm::vec3(s[0], s[1], s[2]));
                }
            }

            auto it = materials.find(materialName);
            if (it == materials.end()) {
                std::cerr << "⚠️  Material '" << materialName << "' not found for object " << name << "\n";
                continue;
            }
            auto mat = it->second;

            if (type == "cube") {
                auto instance = std::make_unique<Cube>(mat);
                world.emplace<Transform>(std::move(instance), transform);
            }
            else if (type == "sphere") {
                float radius = obj["radius"] ? obj["radius"].as<float>() : 1.0f;
                auto position = obj["position"] ? obj["position"].as<std::vector<float>>() : std::vector<float>{0,0,0};
                auto instance = std::make_unique<Sphere>(glm::vec3(position[0], position[1], position[2]), mat, radius);

                // Only use Transform if scale/rotation is specified
                if (obj["transform"]) {
                    world.emplace<Transform>(std::move(instance), transform);
                } else {
                    world.emplace<Sphere>(glm::vec3(position[0], position[1], position[2]), mat, radius);
                }
            }
            else if (type == "uvsphere") {
                int slices = obj["slices"] ? obj["slices"].as<int>() : 16;
                int stacks = obj["stacks"] ? obj["stacks"].as<int>() : 16;
                auto instance = std::make_unique<UVSphere>(slices, stacks, mat);
                world.emplace<Transform>(std::move(instance), transform);
            }
            else if (type == "cone") {
                int resolution = obj["resolution"] ? obj["resolution"].as<int>() : 20;
                float radius = obj["radius"] ? obj["radius"].as<float>() : 1.0f;
                float height = obj["height"] ? obj["height"].as<float>() : 1.0f;
                auto instance = std::make_unique<Cone>(resolution, radius, height, mat);
                world.emplace<Transform>(std::move(instance), transform);
            }else if (type == "quad") {
                auto a = glm::vec3(obj["a"][0].as<float>(), obj["a"][1].as<float>(), obj["a"][2].as<float>());
                auto b = glm::vec3(obj["b"][0].as<float>(), obj["b"][1].as<float>(), obj["b"][2].as<float>());
                auto c = glm::vec3(obj["c"][0].as<float>(), obj["c"][1].as<float>(), obj["c"][2].as<float>());
                auto d = glm::vec3(obj["d"][0].as<float>(), obj["d"][1].as<float>(), obj["d"][2].as<float>());
                if (obj["transform"]) {
                    auto instance = std::make_unique<Quadrilateral>(a, b, c, d, mat);
                    world.emplace<Transform>(std::move(instance), transform);
                } else {
                    world.emplace<Quadrilateral>(a, b, c, d, mat);
                }
            }
            else if (type == "triangle") {
                auto a = glm::vec3(obj["a"][0].as<float>(), obj["a"][1].as<float>(), obj["a"][2].as<float>());
                auto b = glm::vec3(obj["b"][0].as<float>(), obj["b"][1].as<float>(), obj["b"][2].as<float>());
                auto c = glm::vec3(obj["c"][0].as<float>(), obj["c"][1].as<float>(), obj["c"][2].as<float>());
                if (obj["transform"]) {
                    auto instance = std::make_unique<Triangle>(a, b, c, mat);
                    world.emplace<Transform>(std::move(instance), transform);
                } else {
                    world.emplace<Triangle>(a, b, c, mat);
                }
            }

            else {
                std::cerr << "⚠️  Unsupported object type: " << type << "\n";
            }
            std::println("- {} ({}, mat={}, transform={}) ", name, type, materialName, transform);
        }

    }
#pragma endregion
    export class Raytracer : public GameInterface {
    public:
        explicit Raytracer(std::filesystem::path scenePath)
            : m_scenePath(std::move(scenePath)) {
        }

        void Initialize(GLFWwindow *window) override;

        void Render(float aspectRatio) override;

        void ClearResources() override;

        void Update(double deltaTime) override;

        void RenderIMGui();

	    void SetRenderResolution(const int width,  const int height) {
	        if (m_imageWidth != width || m_imageHeight != height) {
	            m_imageWidth = width;
	            m_imageHeight = height;
	            m_rayTraceImage.resize(m_imageWidth * m_imageHeight * 4);  // Resize internal buffer
	            m_renderer.UpdateTexture(m_rayTraceImage, width, height);                    // Notify renderer too
	            m_camera.aspectRatio = static_cast<float>(width) / height; // Keep camera aspect up-to-date
	            m_camera.updateImagePlane();
	        }
	    }


        void ReloadScene() {
            world = {};
            aabb_debug_overlay = {}; // <--- Clear AABB overlay too
            LoadSceneFromYaml(m_scenePath, world, m_camera);
            if (m_renderMode == RenderMode::aabb_debug)
                AddAABBDebugBoxes(world, aabb_debug_overlay);
            std::println("🔄 Scene reloaded.");
        }
        void AddAABBDebugBoxes(const HitableList& world, HitableList& overlay) {
            AABB box;
            for (const auto& obj : world.getObjects()) {
                if (obj->boundingBox(box)) {
                    overlay.emplace<DebugAABB>(box);
                }
            }
        }
	    void SaveImageToFile(const std::string& filename) {
            if (m_rayTraceImage.empty()) {
                std::cerr << "❌ No image data to save!\n";
                return;
            }
            stbi_flip_vertically_on_write(true);
            if (stbi_write_png(filename.c_str(), m_imageWidth, m_imageHeight, 4, m_rayTraceImage.data(), m_imageWidth * 4)) {
                std::println("✅ Saved image to '{}'", filename);
            } else {
                std::cerr << "❌ Failed to save image to file!\n";
            }
        }


    private:
        RaytracerRenderer m_renderer;
        InputSystem m_input;
        Camera m_camera;

        glm::vec3 color(const Ray &r, int depth);

        HitableList world{};
        HitableList aabb_debug_overlay;

        std::filesystem::path m_scenePath;

        RenderMode m_renderMode = RenderMode::diffuse;
        int samples = 1;

        glm::vec3 colorModeNormal(const Ray &r);

        // Image buffer for ray tracing output
        vector<uint8_t> m_rayTraceImage;
        int m_imageWidth{800};
        int m_imageHeight{600};
	    glm::vec3 backgroundColor {0,0,0};
	    int maxDepth = 10;
	    bool useSkyGradient = false;

        void GenerateRayTraceImage(); // Ray tracing function
        // Define camera properties
    };


#pragma region Program Setup


    //************************************
    // Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
    //************************************
    void Raytracer::ClearResources() {
        m_renderer.ClearResources();
    }

    //************************************
    // Set up Keyboard Observer, Initialize the Renderer and Initialize the Skeleton.
    //************************************
    void Raytracer::Initialize(GLFWwindow *window) {
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
        LoadSceneFromYaml(m_scenePath, world, m_camera);
        if (m_renderMode == RenderMode::aabb_debug)
            AddAABBDebugBoxes(world,  aabb_debug_overlay);

        m_renderer.Initialize(800, 600);
    }

    //************************************
    // Calculate mvp matrix, calculate and render joint transforms and calculate and render skin using the boneModelMatrices.
    //************************************
    void Raytracer::Render(float aspectRatio) {
        glm::mat4 Projection = m_camera.GetProjectionMatrix(aspectRatio);
        glm::mat4 View = m_camera.GetViewMatrix();
        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 mvp = Projection * View * Model;
        // Render the ray-traced texture as a full-screen quad
        m_renderer.RenderRayTraceTexture();

        if (m_renderMode == RenderMode::aabb_debug) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            for (const auto& obj : aabb_debug_overlay.getObjects()) {
                AABB box;
                if (obj->boundingBox(box)) {
                    m_renderer.RenderBoundingBox(box, mvp);
                }
            }


        }
        glEnable(GL_DEPTH_TEST);

        RenderIMGui();
    }

    //************************************
    // Handle Input and Update Animation
    //************************************
    void Raytracer::Update(double deltaTime) {
        m_input.Update();
        bool rotateLeft = m_input.IsKeyDown(GLFW_KEY_LEFT);
        bool rotateRight = m_input.IsKeyDown(GLFW_KEY_RIGHT);
        bool zoomIn = m_input.IsKeyDown(GLFW_KEY_UP);
        bool zoomOut = m_input.IsKeyDown(GLFW_KEY_DOWN);
        if (m_input.WasKeyPressed(GLFW_KEY_R)) {
            ReloadScene();
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
                             m_input.IsKeyDown(GLFW_KEY_Q), m_input.IsKeyDown(GLFW_KEY_E),
                             rotateLeft, rotateRight,
                             zoomIn, zoomOut);

        // Update the camera with input flags
        m_camera.Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
        // Generate the ray traced image on the CPU
        //GenerateRayTraceImage();

        // Update the GPU texture with the new image

        m_renderer.UpdateTexture(m_rayTraceImage, m_imageWidth, m_imageHeight);
    }


    void Raytracer::RenderIMGui() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("Raytracing Stats");
            ImGui::Text("Render Mode: %s", m_camera.DebugMode().c_str());
            ImGui::SliderInt("Samples per Pixel", &samples, 1, 50);
            ImGui::SliderInt("Max Depth", &maxDepth, 1, 50);
            ImGui::Checkbox("Use Sky Gradient", &useSkyGradient);
            if (!useSkyGradient) {
                ImGui::ColorPicker3("Background Color", glm::value_ptr(backgroundColor));
            }
            ImGui::Separator();

            ImGui::Text("Resolution %d x %d", m_imageWidth, m_imageHeight );
            static char filename[128] = "Checker_Textures_50_20.png";
            ImGui::InputText("Output Filename", filename, IM_ARRAYSIZE(filename));
            if (ImGui::Button("Render & Save")) {
                GenerateRayTraceImage();
                SaveImageToFile(filename);
            }


            if (ImGui::Button("Render Scene"))
                GenerateRayTraceImage();
            if (ImGui::Button("Reload Scene")) {
                ReloadScene();
            }
            int mode = static_cast<int>(m_renderMode);
            if (ImGui::Combo("Render Mode", &mode, renderModeNames, IM_ARRAYSIZE(renderModeNames))) {
                m_renderMode = static_cast<RenderMode>(mode);
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
#pragma endregion
#pragma region Ray Tracing



    glm::vec3 skyGradient(const Ray &r) {
        glm::vec3 unit_direction = glm::normalize(r.direction());
        float t = 0.5f * (unit_direction.y + 1.0f);
        return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
    }

    glm::vec3 Raytracer::color(const Ray &r, int depth) {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return {0.0f, 0.0f, 0.0f};
        HitRecord rec{};

        if (!world.hit(r, Interval(0.001f, numeric_limits<float>::infinity()), rec)) {
            if (useSkyGradient)
                return skyGradient(r);
            else
                return backgroundColor;
        }




            if (m_renderMode == RenderMode::blinn_phong) {
                auto mat = std::dynamic_pointer_cast<BlinnPhongMaterial>(rec.mat_ptr);
                if (mat) {
                    std::vector<Light> lights = {
                        Light{glm::vec3(-1, -1, -1), glm::vec3(1.0f)},
                        Light{glm::vec3(-0.2, -0, -2), glm::vec3(0.7f)}
                    };
                    return mat->shade(r, rec, lights, world);
                }
                else {
                    return glm::vec3(1.0f, 0.0f, 1.0f); // Magenta = warning
                }
            }


        Ray scattered{};
        glm::vec3 attenuation{};
        glm::vec3 color_from_emission = rec.mat_ptr->emitted();

        if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        glm::vec3 color_from_scatter =  attenuation * color(scattered, depth - 1);
        return color_from_emission + color_from_scatter;

    }

    glm::vec3 Raytracer::colorModeNormal(const Ray &r) {
        HitRecord rec{};
        if (world.hit(r, Interval(0.001f, numeric_limits<float>::infinity()), rec)) {
            return 0.5f * (rec.normal + glm::vec3(1.0f));
        } else {
            return skyGradient(r);
        }
    }


    void Raytracer::GenerateRayTraceImage() {
        m_rayTraceImage.resize(m_imageWidth * m_imageHeight * 4); // RGBA

        // Precompute denominators as floats:
        float invW = 1.0f / float(m_imageWidth);
        float invH = 1.0f / float(m_imageHeight);

        std::atomic<int> linesRemaining = m_imageHeight;
#pragma omp parallel for schedule(dynamic, 1)
        for (int y = 0; y < m_imageHeight; ++y) {
            int remaining = --linesRemaining;
            if (omp_get_thread_num() == 0) {
                std::clog << "\rScanlines remaining: " << remaining << ' ' << std::flush;
            }
            for (int x = 0; x < m_imageWidth; ++x) {
                glm::vec3 col {0.0f};
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
                            col += color(ray, maxDepth);
                            break;
                        default:
                            col += color(ray, maxDepth);
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
        std::clog << "\rDone.                 \n";
    }

#pragma endregion
}
