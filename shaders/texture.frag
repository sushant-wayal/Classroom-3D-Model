#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D tex0;
uniform int hasTexture;
uniform int isWhitePlastic;  // New uniform for white plastic material
uniform vec3 materialDiffuse; // Material diffuse color from MTL file

void main()
{
    vec3 objectColor;
    
    // Check if white plastic material should be applied
    if (isWhitePlastic == 1) {
        // White plastic color
        objectColor = vec3(0.95f, 0.95f, 0.95f);
    } else if (hasTexture == 1) {
        // Use texture if available - PRIORITY OVER FALLBACK COLORS
        objectColor = texture(tex0, TexCoord).rgb;
    } else {
        // Use material diffuse color from MTL file if no texture
        objectColor = materialDiffuse;
    }
    
    // Balanced ambient lighting - enough to see details but preserves color integrity
    float ambientStrength = 0.06;  // Increased from 0.03 to better illuminate furniture
    vec3 ambient = ambientStrength * lightColor;
    
    // Smooth diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate lighting contribution from both lights with REDUCED ATTENUATION
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    
    // Enhanced specular for white plastic and metallic objects
    float specularStrength = 0.6;
    if (isWhitePlastic == 1) {
        // White plastic projector - moderate shiny specular
        specularStrength = 0.5;
    } else if (hasTexture == 1) {
        // Textured objects - low specular to preserve texture detail
        specularStrength = 0.15;
    } else if (FragPos.y < 4.0) {
        // Reduce specular for non-metallic objects
        specularStrength = 0.2;
    }
    
    // Light 1 - Reduced attenuation for better coverage
    vec3 lightDir1 = normalize(lightPos1 - FragPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    float distance1 = length(lightPos1 - FragPos);
    // REDUCED attenuation - lights reach farther
    float attenuation1 = 1.0 / (1.0 + 0.045 * distance1 + 0.0075 * distance1 * distance1);
    diffuse += diff1 * lightColor * attenuation1 * 1.6;  // Good illumination intensity
    
    vec3 reflectDir1 = reflect(-lightDir1, norm);
    float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), 32);
    specular += specularStrength * spec1 * lightColor * attenuation1;
    
    // Light 2 - Reduced attenuation for better coverage
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    float distance2 = length(lightPos2 - FragPos);
    // REDUCED attenuation - lights reach farther
    float attenuation2 = 1.0 / (1.0 + 0.045 * distance2 + 0.0075 * distance2 * distance2);
    diffuse += diff2 * lightColor * attenuation2 * 1.6;  // Good illumination intensity
    
    vec3 reflectDir2 = reflect(-lightDir2, norm);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 32);
    specular += specularStrength * spec2 * lightColor * attenuation2;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    
    // Clamp to prevent over-brightness
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}