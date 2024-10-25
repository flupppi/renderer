// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "BillboardRenderer.h"



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

}

void BillboardRenderer::InitQuad(const Quad& quad) {
	unsigned int indices[] = {
	0, 1, 2, // first Triangle
	2, 3, 0	 // second Triangle
	};
	glGenBuffers(1, &m_quadVertexBufferObject);
	glGenBuffers(1, &m_quadIndexBufferObject);

	// Joint
	glBindVertexArray(m_arrayBufferObjects[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad.vertices), &quad.vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadIndexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
}
//************************************
// Load the shader programs from their glsl files using the ShaderUtil.
//************************************
void BillboardRenderer::LoadShaders()
{


	m_shaderProgram[0] = ShaderUtil::CreateShaderProgram("shaders/VJoint.glsl", "shaders/FJoint.glsl", nullptr);
	m_skeletonTransformLocation = glGetUniformLocation(m_shaderProgram[0], "transformation");


}
