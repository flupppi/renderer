#version 330


in vec4 vertexColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D DecalTexture;
uniform sampler2D GlossTexture;
uniform sampler2D AOTexture;

void main()
{
	color = vertexColor;
}
