#pragma once
#include<vector>
#include<glm/glm.hpp>
#include"../Face.h"

namespace Engine {

	class Plane
	{
	public:
		glm::vec3 normal;
		float originOffset;
		std::vector<Face> coveredFaces;
	};
}
