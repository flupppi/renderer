// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "BillboardRenderer.h"



//************************************
// Load and Initialize all Index and Vertex buffer Objects, Shaders, Vertex Array Objects and Textures that are needed for rendering the Model.
//************************************
void BillboardRenderer::Initialize()
{
	std::vector<SkinMesh> SkinVertices = ReadVertexBufferMesh("./input/Geometrie/MarsienneSkin_VB.raw");
	std::vector<unsigned int> SkinIndices = ReadIndexBuffer("./input/Geometrie/MarsienneSkin_IB.raw");
	std::vector<Vertex> UpperJawVertices = ReadVertexBuffer("./input/Geometrie/UpperJaw_VB.raw");
	std::vector<unsigned int> UpperJawIndices = ReadIndexBuffer("./input/Geometrie/UpperJaw_IB.raw");
	std::vector<Vertex> LowerJawVertices = ReadVertexBuffer("./input/Geometrie/LowerJaw_VB.raw");
	std::vector<unsigned int> LowerJawIndices = ReadIndexBuffer("./input/Geometrie/LowerJaw_IB.raw");
	std::vector<Vertex> RtEyeBallVertices = ReadVertexBuffer("./input/Geometrie/RtEyeBall_smtt_VB.raw");
	std::vector<unsigned int> RtEyeBallIndices = ReadIndexBuffer("./input/Geometrie/RtEyeBall_smtt_IB.raw");
	std::vector<Vertex> LtEyeBallVertices = ReadVertexBuffer("./input/Geometrie/LtEyeBall_smtt_VB.raw");
	std::vector<unsigned int> LtEyeBallIndices = ReadIndexBuffer("./input/Geometrie/LtEyeBall_smtt_IB.raw");
	std::vector<Vertex> RtEyeTranspVertices = ReadVertexBuffer("./input/Geometrie/RtEyeTransp_smtt_VB.raw");
	std::vector<unsigned int> RtEyeTranspIndices = ReadIndexBuffer("./input/Geometrie/RtEyeTransp_smtt_IB.raw");
	std::vector<Vertex> LtEyeTranspVertices = ReadVertexBuffer("./input/Geometrie/LtEyeTransp_smtt_VB.raw");
	std::vector<unsigned int> LtEyeTranspIndices = ReadIndexBuffer("./input/Geometrie/LtEyeTransp_smtt_IB.raw");

	LoadShaders();

	GLuint vertexBufferObjects[6], elementBufferObjects[6];
	glGenVertexArrays(8, m_arrayBufferObjects);
	glGenBuffers(1, &m_vertexBufferObject);
	glGenBuffers(1, &m_elementBufferObject);
	glGenBuffers(6, vertexBufferObjects);
	glGenBuffers(6, elementBufferObjects);

	// Skin Mesh
	glBindVertexArray(m_arrayBufferObjects[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, SkinVertices.size() * sizeof(SkinMesh), &SkinVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, SkinIndices.size() * sizeof(unsigned int), &SkinIndices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, texCoord));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, boneIndex));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, skinWeights));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, positionOffset0));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, normalOffset0));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, positionOffset1));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(SkinMesh), (void*)offsetof(SkinMesh, normalOffset1));
	glEnableVertexAttribArray(8);


	// Textures
	glGenTextures(3, m_skinTextures);

	// Decal Texture
	glBindTexture(GL_TEXTURE_2D, m_skinTextures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(GL_TRUE); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("./input/Texturen/SkinTex_Decal.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// Gloss Texture
	glBindTexture(GL_TEXTURE_2D, m_skinTextures[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(GL_TRUE); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load("./input/Texturen/SkinTex_Gloss.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// AO Texture
	glBindTexture(GL_TEXTURE_2D, m_skinTextures[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(GL_TRUE); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load("./input/Texturen/SkinTex_AO.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);



	// Upper Jaw
	glBindVertexArray(m_arrayBufferObjects[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
	glBufferData(GL_ARRAY_BUFFER, UpperJawVertices.size() * sizeof(Vertex), &UpperJawVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, UpperJawIndices.size() * sizeof(unsigned int), &UpperJawIndices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	// Lower Jaw
	glBindVertexArray(m_arrayBufferObjects[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);
	glBufferData(GL_ARRAY_BUFFER, LowerJawVertices.size() * sizeof(Vertex), &LowerJawVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, LowerJawIndices.size() * sizeof(unsigned int), &LowerJawIndices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	// RtEyeBall
	glBindVertexArray(m_arrayBufferObjects[3]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, RtEyeBallVertices.size() * sizeof(Vertex), &RtEyeBallVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, RtEyeBallIndices.size() * sizeof(unsigned int), &RtEyeBallIndices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	// LtEyeBall
	glBindVertexArray(m_arrayBufferObjects[4]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[3]);
	glBufferData(GL_ARRAY_BUFFER, LtEyeBallVertices.size() * sizeof(Vertex), &LtEyeBallVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, LtEyeBallIndices.size() * sizeof(unsigned int), &LtEyeBallIndices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);

	// RtEyeTransp
	glBindVertexArray(m_arrayBufferObjects[5]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[4]);
	glBufferData(GL_ARRAY_BUFFER, RtEyeTranspVertices.size() * sizeof(Vertex), &RtEyeTranspVertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjects[4]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, RtEyeTranspIndices.size() * sizeof(unsigned int), &RtEyeTranspIndices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);



	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Textures
	glGenTextures(2, m_eyeTextures);

	// Decal Texture
	glBindTexture(GL_TEXTURE_2D, m_eyeTextures[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(GL_TRUE); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load("./input/Texturen/EyeTex_Decal00.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	// Rendering Squares to represent the Joints.
	float vertices[] = {
		0.5f, 0.5f, 0.0f, // top right
		0.5f, -0.5f, 0.0f, //bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f // top left
	};

	unsigned int indices[] = {
		0, 1, 2, // first Triangle
		2, 3, 0	 // second Triangle
	};
	glGenBuffers(1, &m_skeletonVertexBufferObject);
	glGenBuffers(1, &m_skeletonIndexBufferObject);

	// Joint
	glBindVertexArray(m_arrayBufferObjects[6]);
	glBindBuffer(GL_ARRAY_BUFFER, m_skeletonVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_skeletonIndexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
	glBindVertexArray(m_arrayBufferObjects[7]);
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
	glUseProgram(m_shaderProgram[4]);
	glUniformMatrix4fv(m_skeletonTransformLocation, 1, GL_FALSE, glm::value_ptr(transformationMatrix));
	glBindVertexArray(m_arrayBufferObjects[7]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}
//************************************
// Render every vertex array object and assign the relevant uniform inputs for each.
//************************************
void BillboardRenderer::RenderSkin(const glm::mat4& transformationMatrix, const std::vector<glm::mat4>& boneModelMatrices, const float expression, GLfloat lightColor[3], GLfloat lightPos[3])
{

	glUseProgram(m_shaderProgram[0]);
	// Define the light position as a variable
	// Get the location of the lightPos uniform variable in the shader program
	GLuint lightPosLocation = glGetUniformLocation(m_shaderProgram[0], "lightPos");
	// Set the value of the lightPos uniform variable
	glUniform3fv(lightPosLocation, 1, lightPos);
	// Light Color
	GLuint lightColorLocation = glGetUniformLocation(m_shaderProgram[0], "lightColor");
	glUniform3fv(lightColorLocation, 1, lightColor);

	glUniformMatrix4fv(m_transformLocation, 1, GL_FALSE, glm::value_ptr(transformationMatrix));
	glUniformMatrix4fv(m_jointTransforms, 9, GL_FALSE, glm::value_ptr(boneModelMatrices[0]));
	auto currentTime = std::chrono::high_resolution_clock::now();
	float elapsedTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	float blendWeight = 0.5f + 0.5f * glm::sin(elapsedTime);
	glUniform1f(glGetUniformLocation(m_shaderProgram[0], "blendWeight"), expression);
	glUniform1i(glGetUniformLocation(m_shaderProgram[0], "DecalTexture"), 0); // set texture uniform manually
	glUniform1i(glGetUniformLocation(m_shaderProgram[0], "GlossTexture"), 1);
	glUniform1i(glGetUniformLocation(m_shaderProgram[0], "AOTexture"), 2);

	// Draw Skin Mesh
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, m_skinTextures[0]);
	glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, m_skinTextures[1]);
	glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, m_skinTextures[2]);
	glBindVertexArray(m_arrayBufferObjects[0]);
	glDrawElements(GL_TRIANGLES, 91932, GL_UNSIGNED_INT, 0);

	glUseProgram(m_shaderProgram[3]);
	GLuint jawLightPosLocation = glGetUniformLocation(m_shaderProgram[0], "lightPos");
	glUniform3fv(jawLightPosLocation, 1, lightPos);
	GLuint jawLightColorLocation = glGetUniformLocation(m_shaderProgram[0], "lightColor");
	glUniform3fv(jawLightColorLocation, 1, lightColor);
	// Draw upper Jaw
	glUniformMatrix4fv(m_teethTransformLocation, 1, GL_FALSE, glm::value_ptr(boneModelMatrices[1]));
	glBindVertexArray(m_arrayBufferObjects[1]);
	glDrawElements(GL_TRIANGLES, 2550, GL_UNSIGNED_INT, 0);
	// Draw lower Jaw
	glUniformMatrix4fv(m_teethTransformLocation, 1, GL_FALSE, glm::value_ptr(boneModelMatrices[6]));
	glBindVertexArray(m_arrayBufferObjects[2]);
	glDrawElements(GL_TRIANGLES, 1560, GL_UNSIGNED_INT, 0);

	glUseProgram(m_shaderProgram[1]);
	glUniformMatrix4fv(m_eyeTransformLocation, 1, GL_FALSE, glm::value_ptr(boneModelMatrices[8]));
	glUniform1i(glGetUniformLocation(m_shaderProgram[1], "texture_diffuse1"), 0); // set it manually
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, m_eyeTextures[0]);
	// Draw Right Eyeball
	glBindVertexArray(m_arrayBufferObjects[3]);
	glDrawElements(GL_TRIANGLES, 5715, GL_UNSIGNED_INT, 0);
	glUniformMatrix4fv(m_eyeTransformLocation, 1, GL_FALSE, glm::value_ptr(boneModelMatrices[7]));

	// Draw Left Eyeball
	glBindVertexArray(m_arrayBufferObjects[4]);
	glDrawElements(GL_TRIANGLES, 5715, GL_UNSIGNED_INT, 0);

	glUseProgram(m_shaderProgram[2]);
	glUniformMatrix4fv(m_eyeTranspTransformLocation, 1, GL_FALSE, glm::value_ptr(boneModelMatrices[8]));
	// Draw Right EyeTransp
	glBindVertexArray(m_arrayBufferObjects[5]);
	glDrawElements(GL_TRIANGLES, 1620, GL_UNSIGNED_INT, 0);
	// Draw Left EyeTransp
	glUniformMatrix4fv(m_eyeTransformLocation, 1, GL_FALSE, glm::value_ptr(boneModelMatrices[7]));

	glBindVertexArray(m_arrayBufferObjects[6]);
	glDrawElements(GL_TRIANGLES, 1620, GL_UNSIGNED_INT, 0);

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
	m_shaderProgram[0] = ShaderUtil::CreateShaderProgram("shaders/VColor.glsl", "shaders/FColor.glsl", "shaders/GHair.glsl");
	m_transformLocation = glGetUniformLocation(m_shaderProgram[0], "transformation");
	m_jointTransforms = glGetUniformLocation(m_shaderProgram[0], "jointTransforms");

	m_shaderProgram[1] = ShaderUtil::CreateShaderProgram("shaders/VEye.glsl", "shaders/FEye.glsl", nullptr);
	m_eyeTransformLocation = glGetUniformLocation(m_shaderProgram[1], "transformation");

	m_shaderProgram[2] = ShaderUtil::CreateShaderProgram("shaders/VEyeTransp.glsl", "shaders/FEyeTransp.glsl", nullptr);
	m_eyeTranspTransformLocation = glGetUniformLocation(m_shaderProgram[2], "transformation");

	m_shaderProgram[3] = ShaderUtil::CreateShaderProgram("shaders/VTeeth.glsl", "shaders/FTeeth.glsl", nullptr);
	m_teethTransformLocation = glGetUniformLocation(m_shaderProgram[3], "transformation");

	m_shaderProgram[4] = ShaderUtil::CreateShaderProgram("shaders/VJoint.glsl", "shaders/FJoint.glsl", nullptr);
	m_skeletonTransformLocation = glGetUniformLocation(m_shaderProgram[4], "transformation");


}
