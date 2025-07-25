// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include "../vendor/stb_image/stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

export module GameRenderer;
import std;
import ShaderUtil;
import Quad;
namespace Engine {

	export class GameRenderer
	{

	public:
		void Initialize();
		void InitQuad(const Quad& quad);
		void RenderQuad(const glm::mat4& transformationMatrix, int usedShader);

		void ClearResources();
		float GetCubieExtension() const { return 2.0f * m_offset; }


	private:
		void LoadShaders();
		const float m_offset = 0.5f;
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();

		GLuint m_skinTextures[3]{ 0 };
		GLuint m_eyeTextures[2]{ 0 };
		GLuint m_arrayBufferObjects[8]{ 0 };
		GLuint m_skeletonVertexBufferObject{ 0 };
		GLuint m_skeletonIndexBufferObject{ 0 };
		GLuint m_quadVertexBufferObject{ 0 };
		GLuint m_quadIndexBufferObject{ 0 };
		GLuint m_vertexBufferObject{ 0 };
		GLuint m_elementBufferObject{ 0 };
		GLuint m_shaderProgram[5]{ 0 };
		GLint  m_transformLocation[5]{ 0 };
		GLuint m_vertexArraySize{ 0 };
		GLuint m_jointTransforms{ 0 };
	};
}

