// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
module;
#pragma once
#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include "vendor/stb_image/stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
export module IRenderer;
import std;
import Quad;
import ShaderUtil;
namespace Engine {

	export class IRenderer
	{
	public:
		virtual void Initialize() = 0;
		virtual void Render() = 0;
		virtual void ClearResources() = 0;
	protected:
		virtual void LoadShaders() = 0;

	};


}