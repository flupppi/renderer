// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include "../vendor/stb_image/stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tiny_gltf.h"

export module BillboardRenderer;
import std;
import IRenderer;
import Quad;
import ShaderUtil;
import Model;
import Plane;
import Camera;
namespace Engine {

	export class BillboardRenderer : IRenderer
	{

	public:
		void Initialize() override;

		std::vector<GLuint> createBufferObjects(const tinygltf::Model &model);

		std::vector<GLuint> createVertexArrayObjects(const tinygltf::Model &model,
		                                             const std::vector<GLuint> &bufferObjects,
		                                             std::vector<VaoRange> &meshToVertexArrays);
		void InitializeGizmo();
		void InitQuad(const Quad& quad);
		void Render() override;
		void RenderQuad(const glm::mat4& transformationMatrix);
		void RenderPlane(const Plane& plane, const glm::mat4& transformationMatrix);
		void RenderGizmo(const glm::mat4& mvp);
		void ClearResources();
		float GetCubieExtension() const { return 2.0f * m_offset; }
		std::unique_ptr<Shader> m_glslProgram;
		GLint modelViewProjMatrixLocation;
		GLint modelViewMatrixLocation;
		GLint normalMatrixLocation;
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
		std::unique_ptr<Shader> m_modelShader;
		std::unique_ptr<Shader> m_redShader;
		std::unique_ptr<Model> m_model;
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

	std::vector<GLuint> BillboardRenderer::createBufferObjects(const tinygltf::Model &model) {
		std::vector<GLuint> bufferObjects(model.buffers.size(), 0);

		glGenBuffers(GLsizei(model.buffers.size()), bufferObjects.data());
		for (size_t i = 0; i < model.buffers.size(); ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[i]);
			glBufferStorage(GL_ARRAY_BUFFER, model.buffers[i].data.size(),
				model.buffers[i].data.data(), 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return bufferObjects;
	}

	std::vector<GLuint> BillboardRenderer::createVertexArrayObjects(const tinygltf::Model &model, const std::vector<GLuint> &bufferObjects, std::vector<VaoRange> &meshToVertexArrays) {
	  std::vector<GLuint> vertexArrayObjects; // We don't know the size yet

	  // For each mesh of model we keep its range of VAOs
	  meshToVertexArrays.resize(model.meshes.size());

	  const GLuint VERTEX_ATTRIB_POSITION_IDX = 0;
	  const GLuint VERTEX_ATTRIB_NORMAL_IDX = 1;
	  const GLuint VERTEX_ATTRIB_TEXCOORD0_IDX = 2;

	  for (size_t i = 0; i < model.meshes.size(); ++i) {
	    const auto &mesh = model.meshes[i];

	    auto &vaoRange = meshToVertexArrays[i];
	    vaoRange.begin =
	        GLsizei(vertexArrayObjects.size()); // Range for this mesh will be at
	                                            // the end of vertexArrayObjects
	    vaoRange.count =
	        GLsizei(mesh.primitives.size()); // One VAO for each primitive

	    // Add enough elements to store our VAOs identifiers
	    vertexArrayObjects.resize(
	        vertexArrayObjects.size() + mesh.primitives.size());

	    glGenVertexArrays(vaoRange.count, &vertexArrayObjects[vaoRange.begin]);
	    for (size_t pIdx = 0; pIdx < mesh.primitives.size(); ++pIdx) {
	      const auto vao = vertexArrayObjects[vaoRange.begin + pIdx];
	      const auto &primitive = mesh.primitives[pIdx];
	      glBindVertexArray(vao);
	      { // POSITION attribute
	        // scope, so we can declare const variable with the same name on each
	        // scope
	        const auto iterator = primitive.attributes.find("POSITION");
	        if (iterator != end(primitive.attributes)) {
	          const auto accessorIdx = (*iterator).second;
	          const auto &accessor = model.accessors[accessorIdx];
	          const auto &bufferView = model.bufferViews[accessor.bufferView];
	          const auto bufferIdx = bufferView.buffer;

	          glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION_IDX);
	          assert(GL_ARRAY_BUFFER == bufferView.target);
	          // Theorically we could also use bufferView.target, but it is safer
	          // Here it is important to know that the next call
	          // (glVertexAttribPointer) use what is currently bound
	          glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);

	          // tinygltf converts strings type like "VEC3, "VEC2" to the number of
	          // components, stored in accessor.type
	          const auto byteOffset = accessor.byteOffset + bufferView.byteOffset;
	          glVertexAttribPointer(VERTEX_ATTRIB_POSITION_IDX, accessor.type,
	              accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
	              (const GLvoid *)byteOffset);
	        }
	      }
	      // todo Refactor to remove code duplication (loop over "POSITION",
	      // "NORMAL" and their corresponding VERTEX_ATTRIB_*)
	      { // NORMAL attribute
	        const auto iterator = primitive.attributes.find("NORMAL");
	        if (iterator != end(primitive.attributes)) {
	          const auto accessorIdx = (*iterator).second;
	          const auto &accessor = model.accessors[accessorIdx];
	          const auto &bufferView = model.bufferViews[accessor.bufferView];
	          const auto bufferIdx = bufferView.buffer;

	          glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL_IDX);
	          assert(GL_ARRAY_BUFFER == bufferView.target);
	          glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);
	          glVertexAttribPointer(VERTEX_ATTRIB_NORMAL_IDX, accessor.type,
	              accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
	              (const GLvoid *)(accessor.byteOffset + bufferView.byteOffset));
	        }
	      }
	      { // TEXCOORD_0 attribute
	        const auto iterator = primitive.attributes.find("TEXCOORD_0");
	        if (iterator != end(primitive.attributes)) {
	          const auto accessorIdx = (*iterator).second;
	          const auto &accessor = model.accessors[accessorIdx];
	          const auto &bufferView = model.bufferViews[accessor.bufferView];
	          const auto bufferIdx = bufferView.buffer;

	          glEnableVertexAttribArray(VERTEX_ATTRIB_TEXCOORD0_IDX);
	          assert(GL_ARRAY_BUFFER == bufferView.target);
	          glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferIdx]);
	          glVertexAttribPointer(VERTEX_ATTRIB_TEXCOORD0_IDX, accessor.type,
	              accessor.componentType, GL_FALSE, GLsizei(bufferView.byteStride),
	              (const GLvoid *)(accessor.byteOffset + bufferView.byteOffset));
	        }
	      }
	      // Index array if defined
	      if (primitive.indices >= 0) {
	        const auto accessorIdx = primitive.indices;
	        const auto &accessor = model.accessors[accessorIdx];
	        const auto &bufferView = model.bufferViews[accessor.bufferView];
	        const auto bufferIdx = bufferView.buffer;

	        assert(GL_ELEMENT_ARRAY_BUFFER == bufferView.target);
	        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
	            bufferObjects[bufferIdx]); // Binding the index buffer to
	                                       // GL_ELEMENT_ARRAY_BUFFER while the VAO
	                                       // is bound is enough to tell OpenGL we
	                                       // want to use that index buffer for that
	                                       // VAO
	      }
	    }
	  }
	  glBindVertexArray(0);

	  std::clog << "Number of VAOs: " << vertexArrayObjects.size() << std::endl;

	  return vertexArrayObjects;

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

	void BillboardRenderer::RenderPlane(const Plane& plane, const glm::mat4& transformationMatrix)
	{
		m_redShader->use();
		m_redShader->setMat4("transformation", transformationMatrix); // Set the MVP matrix

		plane.Draw(*m_redShader);

	}

	void BillboardRenderer::RenderGizmo(const glm::mat4& mvp)
	{
		glUseProgram(m_gizmoShaderProgram);
		glUniformMatrix4fv(m_gizmoMVPUniform, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(m_gizmoVAO);
		glDrawArrays(GL_LINES, 0, 6);  // 6 vertices: 2 for each axis
		glBindVertexArray(0);

		glUseProgram(0);

		glm::mat4 model = glm::mat4(1.0f); // Model matrix
		m_modelShader->use();
		m_modelShader->setMat4("mvp", mvp); // Set the MVP matrix
		m_model->Draw(*m_modelShader);

		m_redShader->use();
		m_redShader->setMat4("transformation", mvp); // Set the MVP matrix

		m_model->boundingBox.Draw(*m_redShader);

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


		stbi_set_flip_vertically_on_load(true);
		// Loader shaders
		m_glslProgram = std::make_unique<Shader>("shaders/VBasic.glsl", "shaders/FBasic.glsl");
		modelViewProjMatrixLocation = glGetUniformLocation(m_glslProgram->ID, "uModelViewProjMatrix");
		modelViewMatrixLocation = glGetUniformLocation(m_glslProgram->ID, "uModelViewMatrix");
		normalMatrixLocation = glGetUniformLocation(m_glslProgram->ID, "uNormalMatrix");


		m_modelShader = std::make_unique<Shader>("shaders/VBasic.glsl", "shaders/FBasic.glsl");

		m_model = std::make_unique<Model>("input/Trees/SingleTree/Tree.obj");
		m_redShader = std::make_unique<Shader>("shaders/VJoint.glsl", "shaders/FJoint.glsl");
		m_shaderProgram[0] = ShaderUtil::CreateShaderProgram("shaders/VJoint.glsl", "shaders/FJoint.glsl", nullptr);
		m_skeletonTransformLocation = glGetUniformLocation(m_shaderProgram[0], "transformation");
		// Load gizmo shader program
		m_gizmoShaderProgram = ShaderUtil::CreateShaderProgram("shaders/VGizmo.glsl", "shaders/FGizmo.glsl", nullptr);
		m_gizmoMVPUniform = glGetUniformLocation(m_gizmoShaderProgram, "mvp");
	}
}
