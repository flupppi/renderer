#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec2 vTexCoords;

uniform vec3 uLightDirection;
uniform vec3 uLightIntensity;

out vec4 fColor;

void main()
{
  vec3 viewSpaceNormal=normalize(vViewSpaceNormal);
  float oneOverPi = 1. / 3.14;
  vec3 pre = vec3(oneOverPi) * uLightIntensity * dot(viewSpaceNormal, uLightDirection);
  fColor=vec4(pre.xyz,1);
}