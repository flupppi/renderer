#version 330

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 Lighting;

uniform mat4 transformation;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{
    gl_Position = transformation * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    FragPos = vec3(transformation * vec4(aPos, 1.0));
    Normal =mat3(transformation) * aNormal;

    // Calculate lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = spec * lightColor;
    Lighting = ambient + diffuse + specular;
}

