//
// Created by flupppi on 23.07.25.
//
module;
#include<glm/glm.hpp>

export module Ray;

namespace Engine {
    export class Ray {
    public:
        glm::vec3 A;
        glm::vec3 B;
        Ray() = default;
        Ray(const glm::vec3& a, const glm::vec3& b){A = a; B = b;}
        glm::vec3 origin() const {return A;}
        glm::vec3 direction() const {return B;}
        glm::vec3 point_at_paramter(float t) const {return A + t * B;}
    };

}