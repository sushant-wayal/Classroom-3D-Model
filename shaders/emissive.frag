#version 330 core
in vec3 vertexColor;

out vec4 FragColor;

void main()
{
    // Emissive material - no lighting calculations, just pure bright color
    // This makes the light panels glow brightly like real light fixtures
    FragColor = vec4(vertexColor, 1.0);
}
