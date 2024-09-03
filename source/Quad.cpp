#include "Quad.h"

/**
 * Default constructor of a four sided polygon
 * 
 */
Quad::Quad(const glm::vec3& v1,const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4)
{
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
	vertices[3] = v4;
}
