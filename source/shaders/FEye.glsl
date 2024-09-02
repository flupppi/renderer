#version 330


in vec4 vertexColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;
void main()
{
	color = texture(texture_diffuse1, TexCoord);
	color = vec4(color.xyz, 1.0);
}

