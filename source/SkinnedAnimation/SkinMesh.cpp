#include "SkinMesh.h"

//************************************
// Read .raw file for the Skin Mesh and return vector that serves as vertex Buffer
//************************************
std::vector<SkinMesh> ReadVertexBufferMesh(const std::string& fileName)
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
std::vector<Vertex> ReadVertexBuffer(const std::string& fileName)
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
std::vector<unsigned int> ReadIndexBuffer(const std::string& fileName)
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