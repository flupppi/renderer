module;
#include <glm/glm.hpp>
#include <GL/glew.h>
export module BoundingBox;
import std;
import ShaderUtil;
namespace Engine {

    export class BoundingBox
    {
    public:
        glm::vec3 minCorner;               // Minimum corner of the bounding box.
        glm::vec3 maxCorner;               // Maximum corner of the bounding box.
        float margin;                      // Optional margin for rendering.

        BoundingBox() : minCorner(0.0f), maxCorner(0.0f), margin(0.0f), VAO(0), VBO(0), EBO(0) {}
        BoundingBox(const glm::vec3& minCorner, const glm::vec3& maxCorner)
            : minCorner(minCorner), maxCorner(maxCorner), margin(0.0f), VAO(0), VBO(0), EBO(0) {
            setupMesh();
        }

        void setupMesh();
        void Draw(Shader& shader) const;
        void expandToFit(const glm::vec3& point);
        bool contains(const glm::vec3& point);
        glm::mat4 calculateTransform() const;

    private:
        unsigned int VAO, VBO, EBO;       // OpenGL buffer objects
        std::vector<glm::vec3> vertices;  // Vertices for the bounding box
        std::vector<unsigned int> indices; // Indices for rendering the bounding box
    };
    void BoundingBox::expandToFit(const glm::vec3& point)
    {
    }
    bool BoundingBox::contains(const glm::vec3& point)
    {
        return false;
    }
    glm::mat4 BoundingBox::calculateTransform() const
    {
        return glm::mat4();
    }

    // Generate and upload the bounding box geometry
    void BoundingBox::setupMesh() {
        // Define vertices for the bounding box
        vertices = {
            minCorner,                                     // 0: Front-bottom-left
            glm::vec3(maxCorner.x, minCorner.y, minCorner.z), // 1: Front-bottom-right
            glm::vec3(maxCorner.x, maxCorner.y, minCorner.z), // 2: Front-top-right
            glm::vec3(minCorner.x, maxCorner.y, minCorner.z), // 3: Front-top-left
            glm::vec3(minCorner.x, minCorner.y, maxCorner.z), // 4: Back-bottom-left
            glm::vec3(maxCorner.x, minCorner.y, maxCorner.z), // 5: Back-bottom-right
            maxCorner,                                     // 6: Back-top-right
            glm::vec3(minCorner.x, maxCorner.y, maxCorner.z)  // 7: Back-top-left
        };

        // Define indices for drawing the bounding box as lines
        indices = {
            0, 1, 1, 2, 2, 3, 3, 0, // Front face
            4, 5, 5, 6, 6, 7, 7, 4, // Back face
            0, 4, 1, 5, 2, 6, 3, 7  // Connecting edges
        };

        // Generate OpenGL buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Upload index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0); // Unbind VAO
    }

    // Render the bounding box
    void BoundingBox::Draw(Shader& shader) const {
        shader.use(); // Activate the shader program
        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
