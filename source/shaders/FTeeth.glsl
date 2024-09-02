#version 330

uniform vec3 lightColor;
uniform vec3 lightPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Lighting;

out vec4 FragColor;

void main()
{
    vec4 textureColor = vec4(1.0f, 1.0f, 1.0f,1.0f);
    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = max(dot(Normal, normalize(lightPos - FragPos)), 0.0) * lightColor;
    vec3 specular = vec3(0.0);

    if (diffuse.r > 0.0) {
        vec3 viewDir = normalize(-FragPos);
        vec3 reflectDir = reflect(-normalize(lightPos - FragPos), normalize(Normal));
        float roughness = 0.02; // Set the roughness to a lower value
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1.0 / roughness);
        specular = spec * lightColor ;
    }

    vec3 lighting = Lighting;
    vec3 result = (ambient + diffuse + specular) * textureColor.rgb;
    
   // FragColor = vec4(result*lighting, textureColor.a);
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}