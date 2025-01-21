#version 330 core

in vec2 TexCoord;         // Interpolated texture coordinates
in vec3 FragPos;          // Interpolated fragment position in world space
in vec3 Normal;           // Interpolated normal vector in world space

uniform sampler2D texture_diffuse1; // Diffuse texture


out vec4 FragColor;

void main() {

    // Combine results with the texture color
    vec4 color = texture(texture_diffuse1, TexCoord);
    
    FragColor = color;

        // Use the y-component of the normal for shading
    //float brightness = 0.5 + 0.5 * Normal.y;

    // Simulate white material
    //vec3 color = vec3(1.0, 1.0, 1.0);  // Plain white
    //FragColor = vec4(color * brightness, 1.0);
}
