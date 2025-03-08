module;
#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module Billboard;
import Plane;
import BillboardTexture;

namespace Engine {

	export class Billboard
	{
	public:
		GLuint textureID;
		glm::mat4 modelMatrix;
		Plane representativePlane;
		void render();
		void updateTexture(const BillboardTexture& texture);
	};
	void Billboard::render()
	{
	}
	void Billboard::updateTexture(const BillboardTexture& texture)
	{
	}
}