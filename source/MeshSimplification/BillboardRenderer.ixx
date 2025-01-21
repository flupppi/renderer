// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include "../vendor/stb_image/stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

export module BillboardRenderer;
import std;
import IRenderer;
import Quad;
import ShaderUtil;
namespace Engine {

	export class BillboardRenderer : IRenderer
	{

	public:
		void Initialize() override;
		void InitializeGizmo();
		void InitQuad(const Quad& quad);
		void Render() override;
		void RenderQuad(const glm::mat4& transformationMatrix);
		void RenderGizmo(const glm::mat4& mvp);

		void ClearResources();
		float GetCubieExtension() const { return 2.0f * m_offset; }


	private:
		void LoadShaders();
		const float m_offset{ 0.5f };
		const std::chrono::time_point<std::chrono::high_resolution_clock> startTime{ std::chrono::high_resolution_clock::now() };

		GLuint m_skinTextures[3]{ 0 };
		GLuint m_eyeTextures[2]{ 0 };
		GLuint m_arrayBufferObjects[8]{ 0 };
		GLuint m_skeletonVertexBufferObject{ 0 };
		GLuint m_skeletonIndexBufferObject{ 0 };

		// For world coordinate system gizmo
		GLuint m_gizmoVAO{ 0 };
		GLuint m_gizmoVBO{ 0 };
		GLuint m_gizmoShaderProgram{ 0 };
		GLuint m_gizmoMVPUniform{ 0 };


		GLuint m_quadVertexBufferObject{ 0 };
		GLuint m_quadIndexBufferObject{ 0 };
		GLuint m_vertexBufferObject{ 0 };
		GLuint m_elementBufferObject{ 0 };
		GLuint m_shaderProgram[5]{ 0 };
		GLint  m_transformLocation{ 0 }, m_eyeTransformLocation{ 0 }, m_eyeTranspTransformLocation{ 0 }, m_teethTransformLocation{ 0 }, m_skeletonTransformLocation{ 0 };
		GLuint m_vertexArraySize{ 0 };
		GLuint m_jointTransforms{ 0 };

	};


	//************************************
// Load and Initialize all Index and Vertex buffer Objects, Shaders, Vertex Array Objects and Textures that are needed for rendering the Model.
//************************************
	void BillboardRenderer::Initialize()
	{

		LoadShaders();

		GLuint vertexBufferObjects[6], elementBufferObjects[6];
		glGenVertexArrays(8, m_arrayBufferObjects);
		glGenBuffers(1, &m_vertexBufferObject);
		glGenBuffers(1, &m_elementBufferObject);
		glGenBuffers(6, vertexBufferObjects);
		glGenBuffers(6, elementBufferObjects);

		// Unbind Buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		InitializeGizmo();

	}

	void BillboardRenderer::InitializeGizmo() {

		// Define vertices for the x, y, and z axes
		std::array<GLfloat, 18> gizmoVertices{ {
				// X-axis line
				0.0f, 0.0f, 0.0f,  // Origin
				1.0f, 0.0f, 0.0f,  // Along x-axis
				// Y-axis line
				0.0f, 0.0f, 0.0f,  // Origin
				0.0f, 1.0f, 0.0f,  // Along y-axis
				// Z-axis line
				0.0f, 0.0f, 0.0f,  // Origin
				0.0f, 0.0f, 1.0f   // Along z-axis
				}
		};

		// Generate and bind VAO and VBO
		glGenVertexArrays(1, &m_gizmoVAO);
		glGenBuffers(1, &m_gizmoVBO);

		glBindVertexArray(m_gizmoVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_gizmoVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoVertices), gizmoVertices.data(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Unbind VAO and VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	void BillboardRenderer::InitQuad(const Quad& quad) {
		glGenBuffers(1, &m_quadVertexBufferObject);
		glGenBuffers(1, &m_quadIndexBufferObject);

		// Joint
		glBindVertexArray(m_arrayBufferObjects[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad.vertices), &quad.vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadIndexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad.indices), &quad.indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Unbind Buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void BillboardRenderer::Render()
	{
	}

	//************************************
	// Render one red square for each joint in the skeleton.
	//************************************
	void BillboardRenderer::RenderQuad(const glm::mat4& transformationMatrix)
	{
		glUseProgram(m_shaderProgram[0]);
		glUniformMatrix4fv(m_skeletonTransformLocation, 1, GL_FALSE, glm::value_ptr(transformationMatrix));
		glBindVertexArray(m_arrayBufferObjects[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}

	void BillboardRenderer::RenderGizmo(const glm::mat4& mvp)
	{
		glUseProgram(m_gizmoShaderProgram);
		glUniformMatrix4fv(m_gizmoMVPUniform, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(m_gizmoVAO);
		glDrawArrays(GL_LINES, 0, 6);  // 6 vertices: 2 for each axis
		glBindVertexArray(0);

		glUseProgram(0);
	}

	//************************************
	// Delete Buffers, VAOs and Shaders
	//************************************
	void BillboardRenderer::ClearResources()
	{
		glDeleteBuffers(1, &m_vertexBufferObject);
		glDeleteBuffers(1, &m_elementBufferObject);
		glDeleteVertexArrays(2, m_arrayBufferObjects);
		glDeleteProgram(m_shaderProgram[0]);
		glDeleteProgram(m_shaderProgram[1]);


		// Cleanup gizmo resources
		glDeleteVertexArrays(1, &m_gizmoVAO);
		glDeleteBuffers(1, &m_gizmoVBO);
		glDeleteProgram(m_gizmoShaderProgram);
	}
	//************************************
	// Load the shader programs from their glsl files using the ShaderUtil.
	//************************************
	void BillboardRenderer::LoadShaders()
	{
		m_shaderProgram[0] = ShaderUtil::CreateShaderProgram("shaders/VJoint.glsl", "shaders/FJoint.glsl", nullptr);
		m_skeletonTransformLocation = glGetUniformLocation(m_shaderProgram[0], "transformation");
		// Load gizmo shader program
		m_gizmoShaderProgram = ShaderUtil::CreateShaderProgram("shaders/VGizmo.glsl", "shaders/FGizmo.glsl", nullptr);
		m_gizmoMVPUniform = glGetUniformLocation(m_gizmoShaderProgram, "mvp");
	}
}
