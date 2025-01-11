#pragma once
#include<glm/glm.hpp>
#include<array>
#include"TransformComponent.h"
/**
 * Four sided polygon class. Represents a cut plane in the Billboard cloud.
 * 
 */
namespace Engine {

	class Quad
	{
	public:
		std::array<glm::vec3, 4> vertices;
		std::array<unsigned int, 6> indices{ { 0, 1, 2, 2, 3, 0} };

		Quad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4);
		Quad();

		TransformComponent trans;
	};

}