#include "LightPanels.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

LightPanels::LightPanels(float roomLength, float roomWidth, float roomHeight, int rows, int cols,
                         int light1Row, int light1Col, int light2Row, int light2Col)
{
    lightVBO = nullptr;
    lightEBO = nullptr;

    // Create dedicated emissive shader for self-glowing effect
    emissiveShader = new Shader("shaders/emissive.vert", "shaders/emissive.frag");

    generateLightPanels(roomLength, roomWidth, roomHeight, rows, cols,
                        light1Row, light1Col, light2Row, light2Col);
    setupLightPanels();
}

void LightPanels::generateLightPanels(float roomLength, float roomWidth, float roomHeight, int rows, int cols,
                                      int light1Row, int light1Col, int light2Row, int light2Col)
{
    // Clear any existing data
    vertices.clear();
    indices.clear();

    // Calculate tile dimensions
    float tileWidth = roomLength / cols;
    float tileHeight = roomWidth / rows;

    std::cout << "Generating glowing light panels at specified tile positions..." << std::endl;

    // Generate Light Panel 1
    float light1X = -roomLength / 2.0f + (light1Col + 0.5f) * tileWidth;
    float light1Z = -roomWidth / 2.0f + (light1Row + 0.5f) * tileHeight;
    glm::vec3 light1Center(light1X, roomHeight, light1Z);
    addLightPanel(light1Center, tileWidth, tileHeight, roomHeight);

    // Generate Light Panel 2
    float light2X = -roomLength / 2.0f + (light2Col + 0.5f) * tileWidth;
    float light2Z = -roomWidth / 2.0f + (light2Row + 0.5f) * tileHeight;
    glm::vec3 light2Center(light2X, roomHeight, light2Z);
    addLightPanel(light2Center, tileWidth, tileHeight, roomHeight);

    std::cout << "Generated " << vertices.size() << " vertices for glowing light panels" << std::endl;
}

void LightPanels::addLightPanel(glm::vec3 center, float panelWidth, float panelHeight, float ceilingHeight)
{
    unsigned int baseIndex = vertices.size();

    // VERY BRIGHT glowing color - pure white with slight warmth
    glm::vec3 glowColor = glm::vec3(2.5f, 2.5f, 2.3f); // Extra bright (>1.0) for emissive glow

    // Panel dimensions - cover almost the entire tile
    float halfWidth = (panelWidth * 0.92f) * 0.5f;   // 92% of tile width
    float halfHeight = (panelHeight * 0.92f) * 0.5f; // 92% of tile height

    // Position just slightly below ceiling level
    float panelY = ceilingHeight - 0.015f; // 1.5cm below ceiling

    // Create a bright glowing rectangular panel (facing down)
    // Bottom face (visible from below - this is the glowing surface)
    vertices.push_back({glm::vec3(center.x - halfWidth, panelY, center.z - halfHeight),
                        glowColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, panelY, center.z - halfHeight),
                        glowColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, panelY, center.z + halfHeight),
                        glowColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, panelY, center.z + halfHeight),
                        glowColor, glm::vec3(0.0f, -1.0f, 0.0f)});

    // Create indices for the rectangular panel
    indices.push_back(baseIndex + 0);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 0);
}

void LightPanels::setupLightPanels()
{
    lightVAO.Bind();

    lightVBO = new VBO((GLfloat *)vertices.data(), vertices.size() * sizeof(LightVertex));
    lightEBO = new EBO(indices.data(), indices.size() * sizeof(unsigned int));

    // Position attribute (location = 0)
    lightVAO.LinkVBOAttrib(*lightVBO, 0, 3, GL_FLOAT, sizeof(LightVertex), (void *)0);
    // Color attribute (location = 1)
    lightVAO.LinkVBOAttrib(*lightVBO, 1, 3, GL_FLOAT, sizeof(LightVertex), (void *)(offsetof(LightVertex, Color)));
    // Normal attribute (location = 2)
    lightVAO.LinkVBOAttrib(*lightVBO, 2, 3, GL_FLOAT, sizeof(LightVertex), (void *)(offsetof(LightVertex, Normal)));

    lightVAO.Unbind();
    lightVBO->Unbind();
    lightEBO->Unbind();
}

void LightPanels::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    // Use emissive shader for self-glowing effect
    emissiveShader->Activate();

    glUniformMatrix4fv(glGetUniformLocation(emissiveShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(emissiveShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(emissiveShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    lightVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    lightVAO.Unbind();
}

void LightPanels::Delete()
{
    lightVAO.Delete();
    if (lightVBO)
    {
        lightVBO->Delete();
        delete lightVBO;
    }
    if (lightEBO)
    {
        lightEBO->Delete();
        delete lightEBO;
    }
    if (emissiveShader)
    {
        emissiveShader->Delete();
        delete emissiveShader;
    }
}
