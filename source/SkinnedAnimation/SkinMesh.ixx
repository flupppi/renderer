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


	//************************************
	// Read .raw file for the Skin Mesh and return vector that serves as vertex Buffer
	//************************************
	export std::vector<SkinMesh> ReadVertexBufferMesh(const std::string& fileName)
	{
		std::vector<SkinMesh> vertices;

		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << fileName << std::endl;
			return vertices;
		}

		file.seekg(0, std::ios::end);
		int fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);

		int numVertices = fileSize / sizeof(SkinMesh);
		vertices.resize(numVertices);

		file.read(reinterpret_cast<char*>(vertices.data()), fileSize);

		file.close();

		return vertices;
	}
	//************************************
	// Read .raw file all the other Meshes and return vector that serves as Vertex Buffer
	//************************************
	export std::vector<Vertex> ReadVertexBuffer(const std::string& fileName)
	{
		std::vector<Vertex> vertices;

		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << fileName << std::endl;
			return vertices;
		}

		file.seekg(0, std::ios::end);
		int fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);

		int numVertices = fileSize / sizeof(Vertex);
		vertices.resize(numVertices);

		file.read(reinterpret_cast<char*>(vertices.data()), fileSize);

		file.close();

		return vertices;
	}
	//************************************
	// Read .raw file for the IB.raw files and return vector serving as index Buffer.
	//************************************
	export std::vector<unsigned int> ReadIndexBuffer(const std::string& fileName)
	{
		std::vector<unsigned int> indices;

		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << fileName << std::endl;
			return indices;
		}

		file.seekg(0, std::ios::end);
		int fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);

		int numIndices = fileSize / sizeof(unsigned int);
		indices.resize(numIndices);

		file.read(reinterpret_cast<char*>(indices.data()), fileSize);

		file.close();

		return indices;
	}

}