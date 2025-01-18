module;
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
module TransformComponent;

namespace Engine {
	void TransformComponent::Update()
	{
	}

	void TransformComponent::Start()
	{
	}

	void TransformComponent::Awake()
	{
	}
	glm::mat4 TransformComponent::GetTransform() const
	{
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_position);
		glm::mat4 rotationMatrix = glm::mat4_cast(glm::quat(glm::radians(m_rotation)));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);

		return translationMatrix * rotationMatrix * scaleMatrix;
	}
}