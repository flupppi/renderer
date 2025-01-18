// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#pragma once
#include <GL/glew.h>

export module ShaderUtil;
import std;
namespace Engine {

	export class ShaderUtil
	{
	public:
		static GLuint CreateShaderProgram(const char* vertexFilename, const char* fragmentFilename, const char* geometryFilename);

	private:
		static std::string LoadFile(const char* fileName);
		static void PrintShaderLog(GLuint shader);
		static void PrintProgramLog(GLuint program);
		static void PrintOGlErrors();
	};

}