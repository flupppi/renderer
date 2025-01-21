#version 330 core

layout(location = 0) in vec3 aPos;         // Position
layout(location = 1) in vec2 aTexCoord;    // Texture coordinates
layout(location = 2) in vec3 aNormal;      // Normal

uniform mat4 mvp;       // Combined Model-View-Projection matrix

out vec2 TexCoord;      // Pass to fragment shader
out vec3 FragPos;       // Fragment position in world space
out vec3 Normal;        // Normal vector in world space

void main() {
    // Transform the position and normal to world space
    FragPos = vec3(mat4(1.0) * vec4(aPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(mat4(1.0)))) * aNormal);

    // Pass texture coordinates to the fragment shader
    TexCoord = aTexCoord;

    // Compute the final vertex position using the MVP matrix
    gl_Position = mvp * vec4(aPos, 1.0);
}
