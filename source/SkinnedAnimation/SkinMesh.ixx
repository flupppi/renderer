module;
#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
export module SkinMesh;
import std;
namespace Engine {
	// Structure holding all data from the Marsienne_Skin.raw
	export struct SkinMesh
	{
		float position[3];
		float texCoord[2];
		float normal[3];
		GLfloat boneIndex[4];
		GLfloat skinWeights[4];
		float blendIndex[3];
		float positionOffset0[3];
		float normalOffset0[3];
		float positionOffset1[3];
		float normalOffset1[3];
	};
	// Structure holding all data other Meshes
	export struct Vertex
	{
		float position[3];
		float texCoord[2];
		float normal[3];
	};


	export std::vector<SkinMesh> ReadVertexBufferMesh(const std::string& fileName);
	export std::vector<Vertex> ReadVertexBuffer(const std::string& fileName);
	export std::vector<unsigned int> ReadIndexBuffer(const std::string& fileName);

}