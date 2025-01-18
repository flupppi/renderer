
module;
#pragma once
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

export module TransformComponent;
import IComponent;
namespace Engine {
    export class TransformComponent : public IComponent
    {
    public:
        TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
            : m_position(position), m_rotation(rotation), m_scale(scale) {
        }
        TransformComponent() : m_position{ 0.0f, 0.0f, 0.0f }, m_rotation{ 0.0f, 0.0f, 0.0f }, m_scale{ 1.0f, 1.0f, 1.0f } {}

        void Update() override;


        void Start() override;


        void Awake() override;

        // Getters and setters for position, rotation, and scale if needed
        const glm::vec3& GetPosition() const { return m_position; }
        const glm::vec3& GetRotation() const { return m_rotation; }
        const glm::vec3& GetScale() const { return m_scale; }

        void SetPosition(const glm::vec3& position) { m_position = position; }
        void SetRotation(const glm::vec3& rotation) { m_rotation = rotation; }
        void SetScale(const glm::vec3& scale) { m_scale = scale; }

        // Calculate and return the transform matrix
        glm::mat4 GetTransform() const;
    private:
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        glm::vec3 m_scale;
    };
}