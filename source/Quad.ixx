module;
#pragma once
#include<glm/glm.hpp>
/**
 * Four sided polygon class. Represents a cut plane in the Billboard cloud.
 * 
 */
export module Quad;
import TransformComponent;
import std;
namespace Engine {

	export class Quad
	{
	public:
		std::array<glm::vec3, 4> vertices;
		std::array<unsigned int, 6> indices{ { 0, 1, 2, 2, 3, 0} };

		Quad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4);
		Quad();

		TransformComponent trans;
	};

}