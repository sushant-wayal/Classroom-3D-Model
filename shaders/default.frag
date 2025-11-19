#version 330 core
in vec3 vertexColor;
in vec3 Normal;
in vec3 FragPos;
in float vertexAlpha;  // Receive alpha from vertex shader

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

// Simple pseudo-random function for rough texture
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
   // Determine surface type and material properties based on position
   float shininess;
   float specularStrength;
   vec3 baseColor = vertexColor;
   
   if (FragPos.y < 0.01) {
       // FLOOR - Shiny white tile (high shininess, strong specular)
       shininess = 128.0;
       specularStrength = 0.8;
   } else if (FragPos.y > 6.5) {
       // CEILING TILES - Shiny plastic white (moderate-high shininess)
       shininess = 64.0;
       specularStrength = 0.6;
   } else {
       // WALLS - Rough painted surface with texture variation
       shininess = 4.0;  // Very rough surface
       specularStrength = 0.05;  // Almost no specular reflection
       
       // Add subtle texture noise to walls for rough painted appearance
       vec2 texCoord = vec2(FragPos.x * 2.0, FragPos.z * 2.0);
       float noise = random(floor(texCoord * 50.0)) * 0.04 - 0.02;
       baseColor = vertexColor + vec3(noise);
       
       // Add very subtle bump variation
       float bumpNoise = random(floor(texCoord * 30.0)) * 0.03;
       baseColor = baseColor * (1.0 + bumpNoise);
   }
   
   // Enhanced ambient lighting for bright classroom
   float ambientStrength = 0.45;
   vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0) * baseColor;
   
   // Smooth diffuse lighting
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   
   // Distance-based attenuation
   float distance = length(lightPos - FragPos);
   float attenuation = 1.0 / (1.0 + 0.035 * distance + 0.0044 * distance * distance);
   
   vec3 diffuse = diff * lightColor * attenuation * 0.8;
   
   // Material-specific specular highlights
   vec3 viewDir = normalize(-FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
   vec3 specular = specularStrength * spec * lightColor * attenuation;
   
   // Add subtle fill light from below for more even lighting
   vec3 upDir = vec3(0.0, 1.0, 0.0);
   float upLight = max(dot(norm, upDir), 0.0) * 0.15;
   vec3 fillLight = upLight * vec3(1.0, 1.0, 0.98) * baseColor;
   
   vec3 result = ambient + diffuse + specular + fillLight;
   
   // Subtle gamma correction for natural appearance
   result = pow(result, vec3(1.0/2.2));
   
   FragColor = vec4(result, vertexAlpha); // Use vertex alpha for transparency
}