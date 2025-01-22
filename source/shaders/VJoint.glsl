#version 330

uniform mat4 transformation;

layout(location = 0) in vec3 aPos;


out vec4 vertexColor;


void main()
{
	gl_Position = transformation * vec4(aPos, 1.0);
	vertexColor = vec4(1.0f, 0.0f, 0.0f, 0.5f);
}
 