module;
#pragma once
#define MAX_BONE_INFLUENCE 4
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
export module Mesh;
import std;
import ShaderUtil;
namespace Engine {    // Base Vertex Structure
	export struct BaseVertex {
		glm::vec3 position;  // Common: Vertex position
		glm::vec2 texCoord;  // Common: Texture coordinates
		glm::vec3 normal;    // Common: Normal vector

		BaseVertex() = default;
		BaseVertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm)
			: position(pos), texCoord(tex), normal(norm) {
		}
	};

	// Extended Vertex for Tangent-Space Lighting
	export struct Vertex : public BaseVertex {
		glm::vec3 tangent;                  // Optional: Tangent for lighting
		glm::vec3 bitangent;                // Optional: Bitangent for lighting
		std::array<int, MAX_BONE_INFLUENCE> m_BoneIDs = { 0 };  // Bone indices
		std::array<float, MAX_BONE_INFLUENCE> m_Weights = { 0.0f }; // Bone weights

		Vertex() = default;
		Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm,
			const glm::vec3& tan, const glm::vec3& bitan)
			: BaseVertex(pos, tex, norm), tangent(tan), bitangent(bitan) {
		}
	};

	export struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};


	// Specialized Vertex for Skinning
	export struct SkinnedMeshVertex : public BaseVertex {
		glm::vec4 boneIndex;               // Bone indices for skinning
		glm::vec4 skinWeights;             // Bone weights for skinning
		glm::vec3 blendIndex;              // Additional blending data
		glm::vec3 positionOffset0;         // Offset for animation
		glm::vec3 normalOffset0;           // Offset for normals
		glm::vec3 positionOffset1;         // Offset for animation
		glm::vec3 normalOffset1;           // Offset for normals

		SkinnedMeshVertex() = default;
		SkinnedMeshVertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& norm,
			const glm::vec4& boneIdx, const glm::vec4& weights,
			const glm::vec3& blendIdx, const glm::vec3& posOffset0, const glm::vec3& normOffset0,
			const glm::vec3& posOffset1, const glm::vec3& normOffset1)
			: BaseVertex(pos, tex, norm), boneIndex(boneIdx), skinWeights(weights),
			blendIndex(blendIdx), positionOffset0(posOffset0), normalOffset0(normOffset0),
			positionOffset1(posOffset1), normalOffset1(normOffset1) {
		}
	};

	//************************************
	// Read .raw file for the Skin Mesh and return vector that serves as vertex Buffer
	//************************************
	export std::vector<SkinnedMeshVertex> ReadVertexBufferMesh(const std::string& fileName)
	{
		std::vector<SkinnedMeshVertex> vertices;

		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << fileName << std::endl;
			return vertices;
		}

		file.seekg(0, std::ios::end);
		int fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);

		int numVertices = fileSize / sizeof(SkinnedMeshVertex);
		vertices.resize(numVertices);

		file.read(reinterpret_cast<char*>(vertices.data()), fileSize);

		file.close();

		return vertices;
	}
	//************************************
	// Read .raw file all the other Meshes and return vector that serves as Vertex Buffer
	//************************************
	export std::vector<BaseVertex> ReadVertexBuffer(const std::string& fileName)
	{
		std::vector<BaseVertex> vertices;

		std::ifstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << fileName << std::endl;
			return vertices;
		}

		file.seekg(0, std::ios::end);
		int fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);

		int numVertices = fileSize / sizeof(BaseVertex);
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

	export class Mesh
	{
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		unsigned int VAO;

		// constructor
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures): vertices(vertices), indices(indices), textures(textures){
			// set the vertex buffers and its attribute pointers.
			setupMesh();
		}

		// render the mesh
		void Draw(Shader& shader)
		{
			// bind appropriate textures
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			for (unsigned int i = 0; i < textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
				// retrieve texture number (the N in diffuse_textureN)
				std::string number;
				std::string name = textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++); // transfer unsigned int to string
				else if (name == "texture_normal")
					number = std::to_string(normalNr++); // transfer unsigned int to string
				else if (name == "texture_height")
					number = std::to_string(heightNr++); // transfer unsigned int to string

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}

			// draw mesh
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// always good practice to set everything back to defaults once configured.
			glActiveTexture(GL_TEXTURE0);
		}

	private:
		// render data 
		unsigned int VBO, EBO;

		// initializes all the buffer objects/arrays
		void setupMesh()
		{
			// create buffers/arrays
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			// load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// set the vertex attribute pointers
			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex texture coords
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
			// vertex normals
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

			// vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
			// ids
			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

			// weights
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
			glBindVertexArray(0);
		}


	};

}