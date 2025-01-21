module;
#pragma once
#include<vector>
#include<glm/glm.hpp>
import Face;

export module Plane;

namespace Engine {

    export class Plane
    {
    public:
        glm::vec3 normal;                  // Plane's normal vector.
        float originOffset;                // Distance from origin.
        glm::vec2 size;                    // Size of the plane in 2D space.
        float priority;                    // Priority for plane selection.
        std::vector<Face> validitySet;     // Faces associated with this plane.
        std::vector<glm::vec2> textureCoords; // Texture coordinates for rendering.

        Plane() : originOffset(0.0f), priority(0.0f), size({ 0.0f, 0.0f }) {}

        // Additional methods
        void calculateSize();
        void updatePriority(float weight);
    };
    void Plane::calculateSize()
    {
    }
    void Plane::updatePriority(float weight)
    {
    }
}
