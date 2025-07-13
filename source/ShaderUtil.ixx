// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
export module ShaderUtil;
import std;
namespace Engine {

	export class ShaderUtil
	{
	public:
		static GLuint CreateShaderProgram(const char* vertexFilename, const char* fragmentFilename, const char* geometryFilename = nullptr);

	private:
		static std::string LoadFile(const char* fileName);
		static void PrintShaderLog(GLuint shader);
		static void PrintProgramLog(GLuint program);
		static void PrintOGlErrors();
	};
	GLuint ShaderUtil::CreateShaderProgram(const char* vertexFilename, const char* fragmentFilename, const char* geometryFilename)
	{
		std::string vertexProgram = LoadFile(vertexFilename);
		const char* vertexString = vertexProgram.c_str();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexString, NULL);
		PrintOGlErrors();
		glCompileShader(vertexShader);
		PrintOGlErrors();
		int vertCompiled;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertCompiled);
		if (vertCompiled != 1)
		{
			std::cout << "Vertex shader error " << std::endl;
			PrintShaderLog(vertexShader);
		}


		std::string fragmentProgram = LoadFile(fragmentFilename);
		const char* fragmentString = fragmentProgram.c_str();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentString, NULL);

		PrintOGlErrors();
		glCompileShader(fragmentShader);
		PrintOGlErrors();
		int fragCompiled;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragCompiled);
		if (fragCompiled != 1)
		{
			std::cout << "Fragment shader error " << std::endl;
			PrintShaderLog(fragmentShader);
		}

		GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		PrintOGlErrors();
		int linked;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
		if (linked != 1)
		{
			std::cout << "Linking failed:" << std::endl;
			PrintProgramLog(shaderProgram);
		}


		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return shaderProgram;

	}

	std::string ShaderUtil::LoadFile(const char* fileName)
	{
		std::string result;
		std::ifstream fileStream(fileName, std::ios::in);
		std::string line;
		while (std::getline(fileStream, line))
		{
			result.append(line + "\n");
		}
		fileStream.close();
		return result;
	}
	void ShaderUtil::PrintShaderLog(GLuint shader)
	{
		int memLength;
		int messageLength;
		char* log;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &memLength);
		if (memLength == 0)
			return;
		log = (char*)malloc(memLength);
		glGetShaderInfoLog(shader, memLength, &messageLength, log);
		std::cout << "Shader Info Log: " << log << std::endl;
		free(log);
	}

	void ShaderUtil::PrintProgramLog(GLuint program)
	{
		int memLength;
		int messageLength;
		char* log;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &memLength);
		if (memLength == 0)
			return;
		log = (char*)malloc(memLength); // TODO: Remove usage of malloc and free here!!
		glGetProgramInfoLog(program, memLength, &messageLength, log);
		std::cout << "Program Info Log: " << log << std::endl;
		free(log);
	}

	void ShaderUtil::PrintOGlErrors()
	{
		int error = glGetError();
		while (error != GL_NO_ERROR)
		{
			std::cout << "OpenGL error: " << error << std::endl;
			error = glGetError();
		}
	}
	static void SetUniform(GLuint programID, const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

static void SetUniform(GLuint programID, const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

static void SetUniform(GLuint programID, const std::string& name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(value));
}

static void SetUniform(GLuint programID, const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}


export class Shader
{
public:
	unsigned int ID;
	Shader()
	{
		ID = ShaderUtil::CreateShaderProgram("shaders/VBasic.glsl", "shaders/FBasic.glsl", nullptr);
		if (ID == 0)
		{
			std::cerr << "ERROR::SHADER::PROGRAM_CREATION_FAILED\n";
		}
	}
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		ID = ShaderUtil::CreateShaderProgram(vertexPath, fragmentPath, geometryPath);
		if (ID == 0)
		{
			std::cerr << "ERROR::SHADER::PROGRAM_CREATION_FAILED\n";
		}
	}

	void use()
	{
		glUseProgram(ID);
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	// Set a 4x4 matrix uniform
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		GLuint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1)
		{
			std::cerr << "WARNING::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
			return;
		}
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
	}
};
}
