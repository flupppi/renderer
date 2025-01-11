#include "Quad.h"
namespace Engine {
	/**
	 * Default constructor of a four sided polygon
	 *
	 */
	Quad::Quad(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec3& v4): trans()
	{
		vertices = { v1, v2, v3, v4 };
	}

	Quad::Quad(): trans() {
		vertices = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.0f, 2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f) };
	}
}