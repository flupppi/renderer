#pragma once
#include<glm/glm.hpp>
/**
 * Four sided polygon class. Represents a cut plane in the Billboard cloud.
 * 
 */
class Quad
{
public:
	glm::vec3 vertices[4];
	Quad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4);

};

