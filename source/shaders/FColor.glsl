#version 330

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Lighting;

out vec4 FragColor;

uniform sampler2D DecalTexture;
uniform sampler2D GlossTexture;
uniform sampler2D AOTexture;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    vec4 textureColor = texture(DecalTexture, TexCoord);
    float glossScale = 2;
    vec4 glossColor = texture(GlossTexture, TexCoord)*glossScale;
    vec4 aoColor = texture(AOTexture, TexCoord);
    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = max(dot(Normal, normalize(lightPos - FragPos)), 0.0) * lightColor;
    vec3 specular = vec3(0.0);

    if (diffuse.r > 0.0) {
        vec3 viewDir = normalize(-FragPos);
        vec3 reflectDir = reflect(-normalize(lightPos - FragPos), normalize(Normal));
        float roughness = 0.02; // Set the roughness to a lower value
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0 / roughness);
        specular = spec * lightColor * glossColor.r;
    }

    vec3 lighting = Lighting;
    vec3 result = (ambient + diffuse + specular) * textureColor.rgb;
    // Ambient occlusion effect
    result *= aoColor.r;
    FragColor = vec4(result*lighting, textureColor.a);
}