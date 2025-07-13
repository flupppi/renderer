// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

export module SemanticSegmentationRenderer;
import ShaderUtil;
import std;
namespace Engine {

	export class SemanticSegmentationRenderer
	{
	public:
		void Initialize();
		void ClearResources();
		void uploadComposite(GLuint& id, const std::vector<uint8_t>& texture, int height, int width);
		void createImage(std::vector<uint8_t> image, int height, int width);
		GLuint m_image{ 0 };        // Main Rendered image
	private:
		void LoadShaders();
		std::vector<GLuint> textures{};
	};


	// --- Creat OpenGL Texture from Image ---
	void SemanticSegmentationRenderer::createImage(std::vector<uint8_t> image, int height, int width) {
		GLuint newImage{ 0 };
		if (!image.empty())
		{
			glGenTextures(1, &newImage);
			glBindTexture(GL_TEXTURE_2D, newImage);

			// Set some texture parameters (repeat, filtering, etc.)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Upload pixel data to the GPU
			glTexImage2D(
				GL_TEXTURE_2D,
				0,               // mip level
				GL_RGBA,         // internal format
				width,
				height,
				0,               // border
				GL_RGBA,         // format of the pixel data
				GL_UNSIGNED_BYTE,
				image.data()   // pointer to data in RAM
			);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			std::cerr << "No valid image data; skipping texture creation.\n";
		}
		m_image = newImage;
	}

	
	void SemanticSegmentationRenderer::uploadComposite(GLuint& id, const std::vector<uint8_t>& texture, int height, int width)
	{	
		// If the composite is empty or invalid, skip
		if (texture.empty())
			return;

		// If we don�t have a texture handle yet, create one
		if (id == 0)
		{
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			textures.push_back(id); // Store the texture handle for later cleanup 
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, id);
		}

		// Upload or update with our composite buffer
		glTexImage2D(
			GL_TEXTURE_2D,
			0,       // mip level
			GL_RGBA, // internal format
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			texture.data()
		);
		glBindTexture(GL_TEXTURE_2D, 0);
		
	}
	//************************************
	// Load and Initialize all Index and Vertex buffer Objects, Shaders, Vertex Array Objects and Textures that are needed for rendering the Model.
	//************************************
	void SemanticSegmentationRenderer::Initialize()
	{
		LoadShaders();
	}

	//************************************
	// Delete Buffers, VAOs and Shaders
	//************************************
	void SemanticSegmentationRenderer::ClearResources()
	{
		for(GLuint texture : textures)
		{
			if (texture)
			{
				glDeleteTextures(1, &texture);
			}
		}
		textures.clear();
	
		if (m_image)
		{
			glDeleteTextures(1, &m_image);
			m_image = 0;
		}
	}
	//************************************
	// Load the shader programs from their glsl files using the ShaderUtil.
	//************************************
	void SemanticSegmentationRenderer::LoadShaders()
	{
	}
}

