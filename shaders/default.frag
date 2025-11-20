#version 330 core
in vec3 vertexColor;
in vec3 Normal;
in vec3 FragPos;
in float vertexAlpha;  // Receive alpha from vertex shader

out vec4 FragColor;

uniform vec3 lightPos1;
uniform vec3 lightPos2;
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
   
   // Reduced ambient lighting to preserve material colors
   float ambientStrength = 0.25;  // Reduced from 0.35 to preserve yellowish wall color
   vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0) * baseColor;
   
   // Smooth diffuse lighting
   vec3 norm = normalize(Normal);
   
   // Calculate lighting contribution from both lights
   vec3 diffuse = vec3(0.0);
   vec3 specular = vec3(0.0);
   
   // Light 1 - Reduced intensity to prevent over-brightness
   vec3 lightDir1 = normalize(lightPos1 - FragPos);
   float diff1 = max(dot(norm, lightDir1), 0.0);
   float distance1 = length(lightPos1 - FragPos);
   float attenuation1 = 1.0 / (1.0 + 0.035 * distance1 + 0.005 * distance1 * distance1);  // Increased attenuation
   diffuse += diff1 * lightColor * attenuation1 * 0.5;  // Reduced from 0.7 to 0.5
   
   vec3 viewDir = normalize(-FragPos);
   vec3 reflectDir1 = reflect(-lightDir1, norm);
   float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), shininess);
   specular += specularStrength * spec1 * lightColor * attenuation1;
   
   // Light 2 - Reduced intensity to prevent over-brightness
   vec3 lightDir2 = normalize(lightPos2 - FragPos);
   float diff2 = max(dot(norm, lightDir2), 0.0);
   float distance2 = length(lightPos2 - FragPos);
   float attenuation2 = 1.0 / (1.0 + 0.035 * distance2 + 0.005 * distance2 * distance2);  // Increased attenuation
   diffuse += diff2 * lightColor * attenuation2 * 0.5;  // Reduced from 0.7 to 0.5
   
   vec3 reflectDir2 = reflect(-lightDir2, norm);
   float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), shininess);
   specular += specularStrength * spec2 * lightColor * attenuation2;
   
   // Add subtle fill light from below for more even lighting
   vec3 upDir = vec3(0.0, 1.0, 0.0);
   float upLight = max(dot(norm, upDir), 0.0) * 0.10;  // Reduced from 0.15
   vec3 fillLight = upLight * vec3(1.0, 1.0, 0.98) * baseColor;
   
   vec3 result = ambient + diffuse + specular + fillLight;
   
   // Subtle gamma correction for natural appearance
   result = pow(result, vec3(1.0/2.2));
   
   FragColor = vec4(result, vertexAlpha); // Use vertex alpha for transparency
}