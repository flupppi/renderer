#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
import Plane;

export module Billboard;
namespace Engine {

	export class Billboard
	{
	public:
		GLuint textureID;
		glm::mat4 modelMatrix;
		Plane representativePlane;
	};
}
 