#version 330

in vec4 vViewSpacePosition;
in vec4 vViewSpaceNormal;
in vec2 vTexCoords;

out vec4 fColor;

void main()
{
   // Need another normalization because interpolation of vertex attributes does not maintain unit length
   vec4 viewSpaceNormal = normalize(vViewSpaceNormal);
   fColor = vec4(viewSpaceNormal.xyz, 1);
}