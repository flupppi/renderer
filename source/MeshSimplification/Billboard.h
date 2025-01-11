#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Plane.h"

namespace Engine {

	class Billboard
	{
	public:
		GLuint textureID;
		glm::mat4 modelMatrix;
		Plane representativePlane;
	};
}
