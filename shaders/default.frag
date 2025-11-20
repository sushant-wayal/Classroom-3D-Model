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
   
   // Balanced ambient lighting - enough to see in corners but preserves blacks
   float ambientStrength = 0.5;  // Increased from 0.015 to light up dark corners
   vec3 ambient = ambientStrength * lightColor * baseColor;
   
   // Smooth diffuse lighting
   vec3 norm = normalize(Normal);
   
   // Calculate lighting contribution from both lights with REDUCED ATTENUATION
   vec3 diffuse = vec3(0.0);
   vec3 specular = vec3(0.0);
   
   // Light 1 - Reduced attenuation so light reaches farther
   vec3 lightDir1 = normalize(lightPos1 - FragPos);
   float diff1 = max(dot(norm, lightDir1), 0.0);
   float distance1 = length(lightPos1 - FragPos);
   // REDUCED attenuation - lights reach much farther into corners
   float attenuation1 = 1.0 / (1.0 + 0.045 * distance1 + 0.0075 * distance1 * distance1);
   diffuse += diff1 * lightColor * attenuation1 * baseColor * 1.5;  // Increased intensity
   
   vec3 viewDir = normalize(-FragPos);
   vec3 reflectDir1 = reflect(-lightDir1, norm);
   float spec1 = pow(max(dot(viewDir, reflectDir1), 0.0), shininess);
   specular += specularStrength * spec1 * lightColor * attenuation1;
   
   // Light 2 - Reduced attenuation so light reaches farther
   vec3 lightDir2 = normalize(lightPos2 - FragPos);
   float diff2 = max(dot(norm, lightDir2), 0.0);
   float distance2 = length(lightPos2 - FragPos);
   // REDUCED attenuation - lights reach much farther into corners
   float attenuation2 = 1.0 / (1.0 + 0.045 * distance2 + 0.0075 * distance2 * distance2);
   diffuse += diff2 * lightColor * attenuation2 * baseColor * 1.5;  // Increased intensity
   
   vec3 reflectDir2 = reflect(-lightDir2, norm);
   float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), shininess);
   specular += specularStrength * spec2 * lightColor * attenuation2;
   
   vec3 result = ambient + diffuse + specular;
   
   // Clamp to prevent over-brightness
   result = clamp(result, 0.0, 1.0);
   
   FragColor = vec4(result, vertexAlpha);
}