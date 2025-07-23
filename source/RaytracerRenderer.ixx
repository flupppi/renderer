// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include "vendor/stb_image/stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

export module RaytracerRenderer;
import Quad;
import ShaderUtil;

import Geometry;
import std;

namespace Engine {

	export class RaytracerRenderer
	{

	public:
		void Initialize();
		void InitializeFullScreenQuad();
		void RenderRayTraceTexture();
		void UpdateTexture(const std::vector<uint8_t>& image, int width, int height);  // New
		void UpdateTextureRow(std::span<const uint8_t> rowData, int x, int y, int width, int height) const
		{
			glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rowData.data());
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		void InitializeAABBRenderer() {
			glGenVertexArrays(1, &m_bboxVAO);
			glGenBuffers(1, &m_bboxVBO);

			glBindVertexArray(m_bboxVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_bboxVBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);
			glBindVertexArray(0);
		}

		void RenderBoundingBox(const AABB& box, glm::mat4 mvp) const {
			glm::vec3 min = box.t0;
			glm::vec3 max = box.t1;

			glm::vec3 corners[] = {
				{min.x, min.y, min.z}, {max.x, min.y, min.z},
				{max.x, max.y, min.z}, {min.x, max.y, min.z},
				{min.x, min.y, max.z}, {max.x, min.y, max.z},
				{max.x, max.y, max.z}, {min.x, max.y, max.z}
			};

			GLuint indices[] = {
				0, 1, 1, 2, 2, 3, 3, 0,
				4, 5, 5, 6, 6, 7, 7, 4,
				0, 4, 1, 5, 2, 6, 3, 7
			};
			glLineWidth(2);
			glm::vec3 lineVertices[24];
			for (int i = 0; i < 24; ++i)
				lineVertices[i] = corners[indices[i]];
			glBindVertexArray(m_bboxVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_bboxVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_DYNAMIC_DRAW);
			glUseProgram(m_bbShader);
			glUniformMatrix4fv(glGetUniformLocation(m_bbShader, "transformation"), 1, GL_FALSE, glm::value_ptr(mvp));
			glDrawArrays(GL_LINES, 0, 24);
			glBindVertexArray(0);
		}

		void ClearResources();


	private:
		void LoadShaders();
		GLuint m_rayTraceTexture{ 0 };  // Texture ID for the ray-traced image
		GLuint m_fullScreenShader{ 0 };
		GLuint m_bbShader{ 0 };
		GLuint m_fullScreenVAO{ 0 };
		GLuint m_fullScreenVBO{ 0 };
		GLuint m_bboxVAO = 0;
		GLuint m_bboxVBO = 0;


	};
//************************************
	// Load and Initialize all Index and Vertex buffer Objects, Shaders, Vertex Array Objects and Textures that are needed for rendering the Model.
	//************************************
	void RaytracerRenderer::Initialize()
	{
		LoadShaders();
		// Unbind Buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// Initialize ray trace texture
		glGenTextures(1, &m_rayTraceTexture);
		glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800,600, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);  // Placeholder size
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		InitializeFullScreenQuad();
		InitializeAABBRenderer();
	}
	void RaytracerRenderer::InitializeFullScreenQuad()
	{
		// Define vertices for a full-screen quad with position and texture coordinates
		float quadVertices[] = {
			// Positions   // TexCoords
			-1.0f,  1.0f,   0.0f, 1.0f,  // Top-left
			-1.0f, -1.0f,   0.0f, 0.0f,  // Bottom-left
			 1.0f, -1.0f,   1.0f, 0.0f,  // Bottom-right

			-1.0f,  1.0f,   0.0f, 1.0f,  // Top-left
			 1.0f, -1.0f,   1.0f, 0.0f,  // Bottom-right
			 1.0f,  1.0f,   1.0f, 1.0f   // Top-right
		};

		// Generate and bind VAO/VBO
		glGenVertexArrays(1, &m_fullScreenVAO);
		glGenBuffers(1, &m_fullScreenVBO);

		glBindVertexArray(m_fullScreenVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_fullScreenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Texture coordinate attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Unbind VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void RaytracerRenderer::UpdateTexture(const std::vector<uint8_t>& image, int width, int height)
	{
		glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void RaytracerRenderer::RenderRayTraceTexture()
	{

		glUseProgram(m_fullScreenShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_rayTraceTexture);
		glUniform1i(glGetUniformLocation(m_fullScreenShader, "rayTraceTexture"), 0);


		glBindVertexArray(m_fullScreenVAO);  // Full-screen quad VAO
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
	//************************************
	// Delete Buffers, VAOs and Shaders
	//************************************
	void RaytracerRenderer::ClearResources()
	{
		// Cleanup full-screen quad resources
		glDeleteVertexArrays(1, &m_fullScreenVAO);
		glDeleteBuffers(1, &m_fullScreenVBO);
		glDeleteProgram(m_fullScreenShader);
	}
	//************************************
	// Load the shader programs from their glsl files using the ShaderUtil.
	//************************************
	void RaytracerRenderer::LoadShaders()
	{
		// Load a full-screen shader program
		m_fullScreenShader = ShaderUtil::CreateShaderProgram("shaders/VFullScreenQuad.glsl", "shaders/FFullScreenQuad.glsl", nullptr);
		m_bbShader =  ShaderUtil::CreateShaderProgram("shaders/VJoint.glsl", "shaders/FJoint.glsl", nullptr);

	}


}