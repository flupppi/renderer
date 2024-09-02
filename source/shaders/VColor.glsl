#version 330
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform mat4 transformation;
uniform mat4 jointTransforms[9];
uniform float blendWeight;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aBoneIndex;
layout(location = 4) in vec4 aBoneWeight;
layout(location = 5) in vec3 aPositionOffset0;
layout(location = 6) in vec3 aNormalOffset0;
layout(location = 7) in vec3 aPositionOffset1;
layout(location = 8) in vec3 aNormalOffset1;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 Lighting; 

void main()
{
    float aBlendWeight = 0.0f;
    vec4 vertexTransform = vec4(0.0f); //+ 0.0f * vec4(aPositionOffset0,1.0f) + 1.0f * vec4(aPositionOffset1, 1.0f);
    vec4 blendedPosition = vec4(0.0);
    vec4 blendedNormal = vec4(0.0f);
    vec4 position0 = vec4(aPos + aPositionOffset0, 1.0);
    vec4 position1 = vec4(aPos + aPositionOffset1, 1.0);
    vec4 normal0 = vec4(aNormal + aNormalOffset0, 1.0);
    vec4 normal1 = vec4(aNormal + aNormalOffset1, 1.0);
    vec4 accumulatedNormal = vec4(0.0f);
    blendedPosition = vec4(aPos, 1.0f) +  position0 * (1.0f - blendWeight) + position1 * blendWeight;
    blendedNormal = vec4(aNormal, 1.0f) +  normal0 * (1.0f - blendWeight) + normal1 * blendWeight;
    // mesh skinning for each vertex
    for (int i = 0; i < 4; i++) {
        float index = aBoneIndex[i];
        int myIndex =int(index);
        float weight = aBoneWeight[i];
        mat4 jointTransform = jointTransforms[myIndex];
        accumulatedNormal += (transpose(inverse(jointTransform))) * blendedNormal * weight;
        vertexTransform += jointTransform * blendedPosition * weight;
    }
	gl_Position = vertexTransform;
    TexCoord = aTexCoord;
    FragPos = vec3(transformation * vec4(aPos, 1.0));
    Normal = normalize(vec3(accumulatedNormal));

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