module;
#pragma once
#include<vector>
#include<glm/glm.hpp>
import Face;

export module Plane;

namespace Engine {

	export class Plane
	{
	public:
		glm::vec3 normal;
		float originOffset;
		std::vector<Face> coveredFaces;
	};
}
