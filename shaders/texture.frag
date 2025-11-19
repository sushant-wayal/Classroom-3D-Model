#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D tex0;
uniform int hasTexture;

void main()
{
    vec3 objectColor;
    
    // Simple texture application - use texture if available, otherwise fallback colors
    if (hasTexture == 1) {
        objectColor = texture(tex0, TexCoord).rgb;
    } else {
        // Determine object color based on position and type
        if (FragPos.y > 4.0) {
            // Fan and ceiling objects - metallic gray
            objectColor = vec3(0.7f, 0.7f, 0.8f);
        } else if (FragPos.y > 1.8) {
            // Desk tops and chair backs - medium brown
            objectColor = vec3(0.8f, 0.6f, 0.4f);
        } else if (FragPos.y > 0.5) {
            // Chair seats - lighter brown
            objectColor = vec3(0.9f, 0.7f, 0.5f);
        } else {
            // Legs and lower parts - dark brown
            objectColor = vec3(0.5f, 0.3f, 0.2f);
        }
    }
    
    // Enhanced lighting for better visibility and smoothness
    float ambientStrength = 0.4;  // Moderate ambient for good contrast
    vec3 ambient = ambientStrength * lightColor;
    
    // Smooth diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.8;
    
    // Enhanced specular for metallic objects (like fans)
    float specularStrength = 0.6;
    if (FragPos.y < 4.0) {
        // Reduce specular for non-metallic objects
        specularStrength = 0.2;
    }
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}