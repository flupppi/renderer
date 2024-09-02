#version 330

uniform mat4 transformation;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec4 vertexColor;
out vec2 TexCoord;

void main()
{
	gl_Position = transformation * vec4(aPos, 1.0);
	vertexColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	TexCoord = aTexCoord;
}
 