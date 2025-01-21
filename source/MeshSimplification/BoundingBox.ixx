module;
#include <glm/glm.hpp>
export module BoundingBox;

namespace Engine {

    export class BoundingBox
    {
    public:
        glm::vec3 minCorner;               // Minimum corner of the bounding box.
        glm::vec3 maxCorner;               // Maximum corner of the bounding box.
        glm::vec2 dimensions;              // Dimensions in texture space.
        float margin;                      // Optional margin for rendering.

        BoundingBox() : margin(0.0f) {}

        void expandToFit(const glm::vec3& point);
        bool contains(const glm::vec3& point);
        glm::mat4 calculateTransform() const;
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
}