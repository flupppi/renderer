//
// Created by flupppi on 22.07.25.
//
module;
#include<glm/glm.hpp>

#include "glm/gtx/component_wise.hpp"

export module Geometry;
import Ray;
import std;

namespace Engine {
    // Slabs Test for Ray Axis-Aligned Bounding Box Intersection according to Marrs et al. 2021
    export bool slabsBoxTest(glm::vec3 p0, glm::vec3 p1, glm::vec3 rayOrigin, glm::vec3 invRayDir, float rayTmin, float rayTmax) {
        glm::vec3 tLower = (p0 - rayOrigin) * invRayDir;
        glm::vec3 tUpper = (p1 - rayOrigin) * invRayDir;

        glm::vec4 tMins = {glm::min(tLower, tUpper), rayTmin};
        glm::vec4 tMaxes = {glm::max(tLower, tUpper), rayTmax};

        float tBoxMin = glm::compMax(tMins);
        float tBoxMax = glm::compMin(tMaxes);
        return tBoxMin <= tBoxMax;
    }
    export class AABB{
    public:
        AABB() = default;
        AABB(glm::vec3 min, glm::vec3 max): t0(min), t1(max) {}
        void FromCenterExtents(glm::vec3 center, glm::vec3 extends) {}
        std::string ToString() const {
            return "AABB[";
        }
        bool hit(const Ray& ray, float t_min, float t_max) const {
            return slabsBoxTest(t0, t1, ray.origin(), 1.0f / ray.direction(), t_min, t_max);
        }

        glm::vec3 t0;
        glm::vec3 t1;
    };

    export class OctreeNode  {
    public:
        OctreeNode() = default;
    };

}