// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <vector>
#include "../vendor/stb_image/stb_image.h"
#include "../ShaderUtil.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream> 
#include <fstream>
#include <chrono>
#include "../Quad.h"
namespace Engine {

	class RaytracerRenderer
	{

	public:
		void Initialize();
		void InitializeGizmo();
		void InitQuad(const Quad& quad);
		void InitializeFullScreenQuad();

		void RenderQuad(const glm::mat4& transformationMatrix);
		void RenderGizmo(const glm::mat4& mvp);
		void RenderRayTraceTexture();
		void UpdateTexture(const std::vector<uint8_t>& image, int width, int height);  // New

		void ClearResources();
		float GetCubieExtension() const { return 2.0f * m_offset; }


	private:
		void LoadShaders();
		const float m_offset = 0.5f;
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();

		GLuint m_skinTextures[3] = { 0 };
		GLuint m_eyeTextures[2] = { 0 };
		GLuint m_arrayBufferObjects[8] = { 0 };
		GLuint m_skeletonVertexBufferObject = 0;
		GLuint m_skeletonIndexBufferObject = 0;

		// For world coordinate system gizmo
		GLuint m_gizmoVAO = 0;
		GLuint m_gizmoVBO = 0;
		GLuint m_gizmoShaderProgram = 0;
		GLuint m_gizmoMVPUniform = 0;


		GLuint m_quadVertexBufferObject = 0;
		GLuint m_quadIndexBufferObject = 0;
		GLuint m_vertexBufferObject = 0;
		GLuint m_elementBufferObject = 0;
		GLuint m_shaderProgram[5] = { 0 };
		GLint  m_transformLocation = 0, m_eyeTransformLocation = 0, m_eyeTranspTransformLocation = 0, m_teethTransformLocation = 0, m_skeletonTransformLocation = 0;
		GLuint m_vertexArraySize = 0;
		GLuint m_jointTransforms = 0;


		GLuint m_rayTraceTexture = 0;  // Texture ID for the ray traced image
		GLuint m_fullScreenShader = 0;
		GLuint m_fullScreenVAO = 0;
		GLuint m_fullScreenVBO = 0;


	};

}