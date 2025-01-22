module;
#pragma once
#include<vector>
#include<glm/glm.hpp>
#include<GL/glew.h>

export module Plane;
import ShaderUtil;
import Face;

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
        // Additional methods
        void calculateSize();
        void updatePriority(float weight);
        Plane() : originOffset(0.0f), size({ 1.0f, 1.0f }), VAO(0), VBO(0), EBO(0) {}
        Plane(glm::vec3 normal, float offset, glm::vec2 size = { 1.0f, 1.0f })
            : normal(normal), originOffset(offset), size(size), VAO(0), VBO(0), EBO(0) {
        }

        void Draw(Shader& shader) const;
        void setupMesh();
    private:
        unsigned int VAO, VBO, EBO; // OpenGL buffer objects
        std::vector<glm::vec3> vertices;  // Vertices for the plane
        std::vector<unsigned int> indices; // Indices for drawing the plane
    };

    // Generate and upload the plane's geometry
    void Plane::setupMesh() {
        // Calculate the plane's vertices
        glm::vec3 center = normal * originOffset;

        // Basis vectors for the plane
        glm::vec3 u = glm::normalize(glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f)));
        if (glm::length(u) < 0.01f) // Avoid parallel vectors
            u = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 v = glm::normalize(glm::cross(normal, u));

        // Scale the basis vectors to match the plane size
        u *= size.x / 2.0f;
        v *= size.y / 2.0f;

        // Define vertices
        vertices = {
            center + u + v,   // Top-right
            center + u - v,   // Bottom-right
            center - u - v,   // Bottom-left
            center - u + v    // Top-left
        };

        // Define indices
        indices = {
            0, 1, 3,  // First triangle
            1, 2, 3   // Second triangle
        };

        // Generate OpenGL buffers and upload data
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0); // Unbind VAO
    }

    // Render the plane
    void Plane::Draw(Shader& shader) const {
        shader.use(); // Activate the shader program
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    void Plane::calculateSize()
    {
    }
    void Plane::updatePriority(float weight)
    {
    }
}
