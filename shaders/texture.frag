#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

#define MAX_LIGHTS 2
uniform vec3 lightPos[MAX_LIGHTS];
// Tube light geometry for area lighting
uniform vec3 tubeCenter;
uniform vec3 tubeAxis;
uniform float tubeLength;
uniform float tubeRadius;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D tex0;
uniform int hasTexture;
uniform int isWhitePlastic;
uniform vec3 materialDiffuse;

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
    vec3 objectColor;
    
    // Check if white plastic material should be applied
    if (isWhitePlastic == 1) {
        objectColor = vec3(0.95f, 0.95f, 0.95f);
    } else if (hasTexture == 1) {
        objectColor = texture(tex0, TexCoord).rgb;
    } else {
        objectColor = materialDiffuse;
    }
    
    // Balanced ambient lighting
    float ambientStrength = 0.06;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    
    // Enhanced specular for white plastic and metallic objects
    float specularStrength = 0.6;
    if (isWhitePlastic == 1) {
        specularStrength = 0.5;
    } else if (hasTexture == 1) {
        specularStrength = 0.15;
    } else if (FragPos.y < 4.0) {
        specularStrength = 0.2;
    }
    
    // Loop through all ceiling panel lights
    for (int i = 0; i < MAX_LIGHTS; i++) {
        vec3 lightDir = normalize(lightPos[i] - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        float distance = length(lightPos[i] - FragPos);
        float attenuation = 1.0 / (1.0 + 0.045 * distance + 0.0075 * distance * distance);
        diffuse += diff * lightColor * attenuation * 1.6;
        
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
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
    diffuse += tubeDiff * lightColor * tubeAttenuation * 2.8;  // Very bright
    
    vec3 tubeReflectDir = reflect(-tubeLightDir, norm);
    float tubeSpec = pow(max(dot(viewDir, tubeReflectDir), 0.0), 32);
    specular += specularStrength * tubeSpec * lightColor * tubeAttenuation * 1.6;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, 1.0);
}