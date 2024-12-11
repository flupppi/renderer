#pragma once
#include"IComponent.h"
#include<glm/glm.hpp>
class TransformComponent : public IComponent
{
public:
    TransformComponent(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
        : m_position(position), m_rotation(rotation), m_scale(scale) {}

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

private:
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
};

