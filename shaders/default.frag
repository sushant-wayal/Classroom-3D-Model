#version 330 core
in vec3 vertexColor;
in vec3 Normal;
in vec3 FragPos;
in float vertexAlpha;  // Receive alpha from vertex shader

out vec4 FragColor;

#define MAX_LIGHTS 2
uniform vec3 lightPos[MAX_LIGHTS];
// Tube light geometry for area lighting
uniform vec3 tubeCenter;
uniform vec3 tubeAxis;
uniform float tubeLength;
uniform float tubeRadius;
uniform vec3 lightColor;

// Simple pseudo-random function for rough texture
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Calculate closest point on tube cylinder surface to fragment
vec3 closestPointOnTube(vec3 fragPos, vec3 tubeCenter, vec3 tubeAxis, float tubeLength, float tubeRadius) {
    vec3 toFrag = fragPos - tubeCenter;
    float alongAxis = dot(toFrag, tubeAxis);
    
    // Clamp to tube length
    float halfLength = tubeLength * 0.5;
    alongAxis = clamp(alongAxis, -halfLength, halfLength);
    
    // Point on tube axis
    vec3 pointOnAxis = tubeCenter + tubeAxis * alongAxis;
    
    // Direction from axis to fragment
    vec3 radialDir = fragPos - pointOnAxis;
    float radialDist = length(radialDir);
    
    // Project onto tube surface
    if (radialDist > 0.001) {
        radialDir = normalize(radialDir) * tubeRadius;
    } else {
        radialDir = vec3(tubeRadius, 0.0, 0.0);
    }
    
    return pointOnAxis + radialDir;
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
   
   // Calculate lighting contribution from ceiling panel lights + tube light array
   vec3 diffuse = vec3(0.0);
   vec3 specular = vec3(0.0);
   
   vec3 viewDir = normalize(-FragPos);
   
   // Loop through all ceiling panel lights
   for (int i = 0; i < MAX_LIGHTS; i++) {
       vec3 lightDir = normalize(lightPos[i] - FragPos);
       float diff = max(dot(norm, lightDir), 0.0);
       float distance = length(lightPos[i] - FragPos);
       float attenuation = 1.0 / (1.0 + 0.045 * distance + 0.0075 * distance * distance);
       diffuse += diff * lightColor * attenuation * baseColor * 1.5;
       
       vec3 reflectDir = reflect(-lightDir, norm);
       float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
       specular += specularStrength * spec * lightColor * attenuation;
   }
   
   // TUBE LIGHT - Area lighting from cylindrical surface
   vec3 closestPoint = closestPointOnTube(FragPos, tubeCenter, tubeAxis, tubeLength, tubeRadius);
   vec3 tubeLightDir = normalize(closestPoint - FragPos);
   float tubeDiff = max(dot(norm, tubeLightDir), 0.0);
   float tubeDistance = length(closestPoint - FragPos);
   
   // Stronger attenuation - light intensity reduces significantly with distance
   // Using standard attenuation: 1 / (constant + linear*d + quadratic*d^2)
   float tubeAttenuation = 1.0 / (1.0 + 0.09 * tubeDistance + 0.032 * tubeDistance * tubeDistance);
   diffuse += tubeDiff * lightColor * tubeAttenuation * baseColor * 2.5;  // Very bright
   
   vec3 tubeReflectDir = reflect(-tubeLightDir, norm);
   float tubeSpec = pow(max(dot(viewDir, tubeReflectDir), 0.0), shininess);
   specular += specularStrength * tubeSpec * lightColor * tubeAttenuation * 1.5;
   
   vec3 result = ambient + diffuse + specular;
   
   // Clamp to prevent over-brightness
   result = clamp(result, 0.0, 1.0);
   
   FragColor = vec4(result, vertexAlpha);
}