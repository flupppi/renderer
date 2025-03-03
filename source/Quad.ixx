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

	Quad::Quad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4) : trans()
	{
		vertices = { v1, v2, v3, v4 };

	}
	Quad::Quad() : trans() {
		vertices = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f) };
	}
}