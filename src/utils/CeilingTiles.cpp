#include "CeilingTiles.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

CeilingTiles::CeilingTiles(float roomLength, float roomWidth, float roomHeight, int rows, int cols)
{
    ceilingVBO = nullptr;
    ceilingEBO = nullptr;
    generateCeilingTiles(roomLength, roomWidth, roomHeight, rows, cols);
    setupCeiling();
}

void CeilingTiles::generateCeilingTiles(float roomLength, float roomWidth, float roomHeight, int rows, int cols)
{
    // Clear any existing data
    vertices.clear();
    indices.clear();

    // Calculate tile dimensions
    float tileWidth = roomLength / cols;
    float tileHeight = roomWidth / rows;

    // Tile properties
    float borderWidth = 0.05f;    // 5cm border width
    float extrusionDepth = 0.03f; // 3cm extrusion depth (subtle)

    // Colors for realistic ceiling tiles
    glm::vec3 tileColor = glm::vec3(0.95f, 0.94f, 0.92f);   // Off-white tile
    glm::vec3 borderColor = glm::vec3(0.85f, 0.83f, 0.80f); // Slightly darker border

    std::cout << "Generating ceiling with " << rows << " rows and " << cols << " columns" << std::endl;
    std::cout << "Tile dimensions: " << tileWidth << " x " << tileHeight << std::endl;

    // Generate tiles
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            // Calculate tile center position
            float x = -roomLength / 2 + (col + 0.5f) * tileWidth;
            float z = -roomWidth / 2 + (row + 0.5f) * tileHeight;
            glm::vec3 tileCenter(x, roomHeight, z);

            addTile(tileCenter, tileWidth, tileHeight, roomHeight, borderWidth, extrusionDepth);
        }
    }

    std::cout << "Generated " << vertices.size() << " vertices and " << indices.size() / 3 << " triangles for ceiling tiles" << std::endl;
}

void CeilingTiles::addTile(glm::vec3 center, float tileWidth, float tileHeight, float ceilingHeight, float borderWidth, float extrusionDepth)
{
    unsigned int baseIndex = vertices.size();

    // Define tile colors
    glm::vec3 tileColor = glm::vec3(0.95f, 0.94f, 0.92f);   // Off-white tile surface
    glm::vec3 borderColor = glm::vec3(0.82f, 0.80f, 0.78f); // Darker border/frame

    // Tile dimensions
    float halfWidth = tileWidth * 0.5f;
    float halfHeight = tileHeight * 0.5f;
    float borderInset = borderWidth;

    // Inner tile dimensions (the actual tile area)
    float innerHalfWidth = halfWidth - borderInset;
    float innerHalfHeight = halfHeight - borderInset;

    // Heights - KEY CHANGE: tile stays at ceiling, only border extrudes down
    float ceilingLevel = center.y;                       // Ceiling level (tile surface here)
    float borderBottomLevel = center.y - extrusionDepth; // Border extends down into room

    // ===== 1. TILE SURFACE (at ceiling level) - stays flush with ceiling =====
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), tileColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), tileColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), tileColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), tileColor, glm::vec3(0.0f, -1.0f, 0.0f)});

    indices.push_back(baseIndex + 0);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 0);
    baseIndex += 4;

    // ===== 2. BORDER FRAMES (extruded downward) - 4 rectangular border strips =====

    // --- TOP BORDER (positive Z) ---
    // Top edge (at ceiling)
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});

    indices.push_back(baseIndex + 0);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 0);

    // Bottom edge (extruded down)
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});

    indices.push_back(baseIndex + 4);
    indices.push_back(baseIndex + 5);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 7);
    indices.push_back(baseIndex + 4);

    // Outer vertical face
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z + halfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});

    indices.push_back(baseIndex + 8);
    indices.push_back(baseIndex + 9);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 11);
    indices.push_back(baseIndex + 8);

    // Inner vertical face
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});

    indices.push_back(baseIndex + 12);
    indices.push_back(baseIndex + 13);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 15);
    indices.push_back(baseIndex + 12);
    baseIndex += 16;

    // --- BOTTOM BORDER (negative Z) ---
    // Top edge
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});

    indices.push_back(baseIndex + 0);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 0);

    // Bottom edge
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});

    indices.push_back(baseIndex + 4);
    indices.push_back(baseIndex + 5);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 7);
    indices.push_back(baseIndex + 4);

    // Outer vertical face
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z - halfHeight), borderColor, glm::vec3(0.0f, 0.0f, -1.0f)});

    indices.push_back(baseIndex + 8);
    indices.push_back(baseIndex + 9);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 11);
    indices.push_back(baseIndex + 8);

    // Inner vertical face
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 0.0f, 1.0f)});

    indices.push_back(baseIndex + 12);
    indices.push_back(baseIndex + 13);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 15);
    indices.push_back(baseIndex + 12);
    baseIndex += 16;

    // --- LEFT BORDER (negative X) ---
    // Top edge
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});

    indices.push_back(baseIndex + 0);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 0);

    // Bottom edge
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});

    indices.push_back(baseIndex + 4);
    indices.push_back(baseIndex + 5);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 7);
    indices.push_back(baseIndex + 4);

    // Outer vertical face
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - halfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});

    indices.push_back(baseIndex + 8);
    indices.push_back(baseIndex + 9);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 11);
    indices.push_back(baseIndex + 8);

    // Inner vertical face
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x - innerHalfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});

    indices.push_back(baseIndex + 12);
    indices.push_back(baseIndex + 13);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 15);
    indices.push_back(baseIndex + 12);
    baseIndex += 16;

    // --- RIGHT BORDER (positive X) ---
    // Top edge
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, -1.0f, 0.0f)});

    indices.push_back(baseIndex + 0);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 0);

    // Bottom edge
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(0.0f, 1.0f, 0.0f)});

    indices.push_back(baseIndex + 4);
    indices.push_back(baseIndex + 5);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 6);
    indices.push_back(baseIndex + 7);
    indices.push_back(baseIndex + 4);

    // Outer vertical face
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + halfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(1.0f, 0.0f, 0.0f)});

    indices.push_back(baseIndex + 8);
    indices.push_back(baseIndex + 9);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 10);
    indices.push_back(baseIndex + 11);
    indices.push_back(baseIndex + 8);

    // Inner vertical face
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z - innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, borderBottomLevel, center.z - innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, borderBottomLevel, center.z + innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});
    vertices.push_back({glm::vec3(center.x + innerHalfWidth, ceilingLevel, center.z + innerHalfHeight), borderColor, glm::vec3(-1.0f, 0.0f, 0.0f)});

    indices.push_back(baseIndex + 12);
    indices.push_back(baseIndex + 13);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 14);
    indices.push_back(baseIndex + 15);
    indices.push_back(baseIndex + 12);
}

void CeilingTiles::setupCeiling()
{
    ceilingVAO.Bind();

    ceilingVBO = new VBO((GLfloat *)vertices.data(), vertices.size() * sizeof(CeilingVertex));
    ceilingEBO = new EBO(indices.data(), indices.size() * sizeof(unsigned int));

    // Position attribute (location = 0)
    ceilingVAO.LinkVBOAttrib(*ceilingVBO, 0, 3, GL_FLOAT, sizeof(CeilingVertex), (void *)0);
    // Color attribute (location = 1)
    ceilingVAO.LinkVBOAttrib(*ceilingVBO, 1, 3, GL_FLOAT, sizeof(CeilingVertex), (void *)(offsetof(CeilingVertex, Color)));
    // Normal attribute (location = 2)
    ceilingVAO.LinkVBOAttrib(*ceilingVBO, 2, 3, GL_FLOAT, sizeof(CeilingVertex), (void *)(offsetof(CeilingVertex, Normal)));

    ceilingVAO.Unbind();
    ceilingVBO->Unbind();
    ceilingEBO->Unbind();
}

void CeilingTiles::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.Activate();

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    ceilingVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    ceilingVAO.Unbind();
}

void CeilingTiles::Delete()
{
    ceilingVAO.Delete();
    if (ceilingVBO)
    {
        ceilingVBO->Delete();
        delete ceilingVBO;
    }
    if (ceilingEBO)
    {
        ceilingEBO->Delete();
        delete ceilingEBO;
    }
}