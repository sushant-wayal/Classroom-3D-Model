#include "TubeLight.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

TubeLight::TubeLight(float roomLength, float roomWidth, float roomHeight)
{
    tubeVBO = nullptr;
    tubeEBO = nullptr;

    // Create dedicated emissive shader for self-glowing effect
    emissiveShader = new Shader("shaders/emissive.vert", "shaders/emissive.frag");

    generateTubeLight(roomLength, roomWidth, roomHeight);
    setupTubeLight();
}

void TubeLight::generateTubeLight(float roomLength, float roomWidth, float roomHeight)
{
    // Clear any existing data
    vertices.clear();
    indices.clear();

    // Position: Right side of ceiling, parallel to right wall, centered between front and back
    float tubeX = roomLength / 2.0f - 1.0f; // 1m from right wall
    float tubeY = roomHeight - 0.1f;        // Just below ceiling
    float tubeZ = 0.0f;                     // Centered between front and back walls

    // Tube dimensions - REDUCED length by 3 times
    tubeLength = (roomWidth * 0.8f) / 3.0f; // Reduced by 3 times
    tubeRadius = 0.05f;                     // 5cm radius (10cm diameter)

    // Store tube geometry for lighting calculations
    tubeCenter = glm::vec3(tubeX, tubeY, tubeZ);
    tubeAxis = glm::vec3(0.0f, 0.0f, 1.0f); // Tube runs along Z-axis

    addTubeLightGeometry(tubeCenter, tubeLength, tubeRadius, roomHeight);
}

void TubeLight::addTubeLightGeometry(glm::vec3 center, float tubeLength, float tubeRadius, float ceilingHeight)
{
    unsigned int baseIndex = vertices.size();

    // EXTREMELY BRIGHT glowing color - much brighter for light source effect
    glm::vec3 glowColor = glm::vec3(4.5f, 4.5f, 4.2f); // Super bright (>1.0) for intense emissive glow

    // Number of segments around the cylinder (increased for smoother appearance)
    int segments = 24; // Increased from 16 for smoother glow
    float halfLength = tubeLength * 0.5f;

    // Create cylindrical tube light (oriented along Z-axis)
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / (float)segments * 2.0f * M_PI;
        float x = cos(angle) * tubeRadius;
        float y = sin(angle) * tubeRadius;

        // Normal pointing outward from cylinder axis
        glm::vec3 normal = glm::normalize(glm::vec3(x, y, 0.0f));

        // Back end of tube
        vertices.push_back({glm::vec3(center.x + x, center.y + y, center.z - halfLength),
                            glowColor, normal});

        // Front end of tube
        vertices.push_back({glm::vec3(center.x + x, center.y + y, center.z + halfLength),
                            glowColor, normal});
    }

    // Create triangles for cylinder body
    for (int i = 0; i < segments; i++)
    {
        unsigned int i0 = baseIndex + i * 2;
        unsigned int i1 = baseIndex + i * 2 + 1;
        unsigned int i2 = baseIndex + (i + 1) * 2;
        unsigned int i3 = baseIndex + (i + 1) * 2 + 1;

        // Two triangles per segment
        indices.push_back(i0);
        indices.push_back(i2);
        indices.push_back(i1);

        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);
    }

    // Add end caps for a complete tube
    unsigned int capBaseIndex = vertices.size();

    // Back cap center
    vertices.push_back({glm::vec3(center.x, center.y, center.z - halfLength),
                        glowColor, glm::vec3(0.0f, 0.0f, -1.0f)});

    // Back cap rim
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / (float)segments * 2.0f * M_PI;
        float x = cos(angle) * tubeRadius;
        float y = sin(angle) * tubeRadius;

        vertices.push_back({glm::vec3(center.x + x, center.y + y, center.z - halfLength),
                            glowColor, glm::vec3(0.0f, 0.0f, -1.0f)});
    }

    // Back cap triangles
    for (int i = 0; i < segments; i++)
    {
        indices.push_back(capBaseIndex);
        indices.push_back(capBaseIndex + i + 1);
        indices.push_back(capBaseIndex + i + 2);
    }

    // Front cap center
    unsigned int frontCapBase = vertices.size();
    vertices.push_back({glm::vec3(center.x, center.y, center.z + halfLength),
                        glowColor, glm::vec3(0.0f, 0.0f, 1.0f)});

    // Front cap rim
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / (float)segments * 2.0f * M_PI;
        float x = cos(angle) * tubeRadius;
        float y = sin(angle) * tubeRadius;

        vertices.push_back({glm::vec3(center.x + x, center.y + y, center.z + halfLength),
                            glowColor, glm::vec3(0.0f, 0.0f, 1.0f)});
    }

    // Front cap triangles
    for (int i = 0; i < segments; i++)
    {
        indices.push_back(frontCapBase);
        indices.push_back(frontCapBase + i + 2);
        indices.push_back(frontCapBase + i + 1);
    }
}

void TubeLight::setupTubeLight()
{
    tubeVAO.Bind();

    tubeVBO = new VBO((GLfloat *)vertices.data(), vertices.size() * sizeof(TubeLightVertex));
    tubeEBO = new EBO(indices.data(), indices.size() * sizeof(unsigned int));

    // Position attribute (location = 0)
    tubeVAO.LinkVBOAttrib(*tubeVBO, 0, 3, GL_FLOAT, sizeof(TubeLightVertex), (void *)0);
    // Color attribute (location = 1)
    tubeVAO.LinkVBOAttrib(*tubeVBO, 1, 3, GL_FLOAT, sizeof(TubeLightVertex), (void *)(offsetof(TubeLightVertex, Color)));
    // Normal attribute (location = 2)
    tubeVAO.LinkVBOAttrib(*tubeVBO, 2, 3, GL_FLOAT, sizeof(TubeLightVertex), (void *)(offsetof(TubeLightVertex, Normal)));

    tubeVAO.Unbind();
    tubeVBO->Unbind();
    tubeEBO->Unbind();
}

void TubeLight::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    // Use emissive shader for self-glowing effect
    emissiveShader->Activate();

    glUniformMatrix4fv(glGetUniformLocation(emissiveShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(emissiveShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(emissiveShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    tubeVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    tubeVAO.Unbind();
}

void TubeLight::Delete()
{
    tubeVAO.Delete();
    if (tubeVBO)
    {
        tubeVBO->Delete();
        delete tubeVBO;
    }
    if (tubeEBO)
    {
        tubeEBO->Delete();
        delete tubeEBO;
    }
    if (emissiveShader)
    {
        emissiveShader->Delete();
        delete emissiveShader;
    }
}
