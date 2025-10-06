#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    vec3 objectColor;
    
    // Determine object color based on position and type
    if (FragPos.z > 7.0) {
        // Blackboard - bright green
        objectColor = vec3(0.0f, 0.6f, 0.0f);
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
    
    // Enhanced lighting for better visibility
    float ambientStrength = 0.6;  // Increased ambient
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * 0.7;
    
    // Reduced specular for wood-like appearance
    float specularStrength = 0.2;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}