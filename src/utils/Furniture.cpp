#include "Furniture.h"
#include <glm/gtc/type_ptr.hpp>

void Furniture::addCube(glm::vec3 position, glm::vec3 size)
{
    glm::vec3 halfSize = size * 0.5f;

    // Define cube vertices relative to position
    std::vector<glm::vec3> cubePositions = {
        // Front face
        position + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z),
        position + glm::vec3(halfSize.x, -halfSize.y, halfSize.z),
        position + glm::vec3(halfSize.x, halfSize.y, halfSize.z),
        position + glm::vec3(-halfSize.x, halfSize.y, halfSize.z),
        // Back face
        position + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z),
        position + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z),
        position + glm::vec3(halfSize.x, halfSize.y, -halfSize.z),
        position + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z)};

    // Normals for each face
    std::vector<glm::vec3> normals = {
        glm::vec3(0.0f, 0.0f, 1.0f),  // Front
        glm::vec3(0.0f, 0.0f, -1.0f), // Back
        glm::vec3(-1.0f, 0.0f, 0.0f), // Left
        glm::vec3(1.0f, 0.0f, 0.0f),  // Right
        glm::vec3(0.0f, -1.0f, 0.0f), // Bottom
        glm::vec3(0.0f, 1.0f, 0.0f)   // Top
    };

    // Texture coordinates
    std::vector<glm::vec2> texCoords = {
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)};

    unsigned int baseIndex = vertices.size();

    // Add vertices for each face
    // Front face
    vertices.push_back({cubePositions[0], normals[0], texCoords[0]});
    vertices.push_back({cubePositions[1], normals[0], texCoords[1]});
    vertices.push_back({cubePositions[2], normals[0], texCoords[2]});
    vertices.push_back({cubePositions[3], normals[0], texCoords[3]});

    // Back face
    vertices.push_back({cubePositions[5], normals[1], texCoords[0]});
    vertices.push_back({cubePositions[4], normals[1], texCoords[1]});
    vertices.push_back({cubePositions[7], normals[1], texCoords[2]});
    vertices.push_back({cubePositions[6], normals[1], texCoords[3]});

    // Left face
    vertices.push_back({cubePositions[4], normals[2], texCoords[0]});
    vertices.push_back({cubePositions[0], normals[2], texCoords[1]});
    vertices.push_back({cubePositions[3], normals[2], texCoords[2]});
    vertices.push_back({cubePositions[7], normals[2], texCoords[3]});

    // Right face
    vertices.push_back({cubePositions[1], normals[3], texCoords[0]});
    vertices.push_back({cubePositions[5], normals[3], texCoords[1]});
    vertices.push_back({cubePositions[6], normals[3], texCoords[2]});
    vertices.push_back({cubePositions[2], normals[3], texCoords[3]});

    // Bottom face
    vertices.push_back({cubePositions[4], normals[4], texCoords[0]});
    vertices.push_back({cubePositions[5], normals[4], texCoords[1]});
    vertices.push_back({cubePositions[1], normals[4], texCoords[2]});
    vertices.push_back({cubePositions[0], normals[4], texCoords[3]});

    // Top face
    vertices.push_back({cubePositions[3], normals[5], texCoords[0]});
    vertices.push_back({cubePositions[2], normals[5], texCoords[1]});
    vertices.push_back({cubePositions[6], normals[5], texCoords[2]});
    vertices.push_back({cubePositions[7], normals[5], texCoords[3]});

    // Add indices for all faces
    for (int face = 0; face < 6; face++)
    {
        unsigned int faceBase = baseIndex + face * 4;
        indices.push_back(faceBase);
        indices.push_back(faceBase + 1);
        indices.push_back(faceBase + 2);
        indices.push_back(faceBase + 2);
        indices.push_back(faceBase + 3);
        indices.push_back(faceBase);
    }
}

Furniture Furniture::createDesk(float width, float height, float depth)
{
    Furniture desk;

    // Desktop
    desk.addCube(glm::vec3(0.0f, height - 0.05f, 0.0f), glm::vec3(width, 0.1f, depth));

    // Legs
    float legWidth = 0.1f;
    float legHeight = height - 0.1f;
    desk.addCube(glm::vec3(-width / 2 + legWidth / 2, legHeight / 2, -depth / 2 + legWidth / 2),
                 glm::vec3(legWidth, legHeight, legWidth));
    desk.addCube(glm::vec3(width / 2 - legWidth / 2, legHeight / 2, -depth / 2 + legWidth / 2),
                 glm::vec3(legWidth, legHeight, legWidth));
    desk.addCube(glm::vec3(-width / 2 + legWidth / 2, legHeight / 2, depth / 2 - legWidth / 2),
                 glm::vec3(legWidth, legHeight, legWidth));
    desk.addCube(glm::vec3(width / 2 - legWidth / 2, legHeight / 2, depth / 2 - legWidth / 2),
                 glm::vec3(legWidth, legHeight, legWidth));

    desk.setupFurniture();
    return desk;
}

Furniture Furniture::createChair(float width, float height, float depth)
{
    Furniture chair;

    // Seat
    float seatHeight = height * 0.5f;
    chair.addCube(glm::vec3(0.0f, seatHeight, 0.0f), glm::vec3(width, 0.1f, depth));

    // Backrest
    chair.addCube(glm::vec3(0.0f, seatHeight + height * 0.3f, -depth / 2 + 0.05f),
                  glm::vec3(width, height * 0.6f, 0.1f));

    // Legs
    float legWidth = 0.05f;
    chair.addCube(glm::vec3(-width / 2 + legWidth / 2, seatHeight / 2, -depth / 2 + legWidth / 2),
                  glm::vec3(legWidth, seatHeight, legWidth));
    chair.addCube(glm::vec3(width / 2 - legWidth / 2, seatHeight / 2, -depth / 2 + legWidth / 2),
                  glm::vec3(legWidth, seatHeight, legWidth));
    chair.addCube(glm::vec3(-width / 2 + legWidth / 2, seatHeight / 2, depth / 2 - legWidth / 2),
                  glm::vec3(legWidth, seatHeight, legWidth));
    chair.addCube(glm::vec3(width / 2 - legWidth / 2, seatHeight / 2, depth / 2 - legWidth / 2),
                  glm::vec3(legWidth, seatHeight, legWidth));

    chair.setupFurniture();
    return chair;
}

Furniture Furniture::createPodium(float width, float height, float depth)
{
    Furniture podium;

    // Main body
    podium.addCube(glm::vec3(0.0f, height / 2, 0.0f), glm::vec3(width, height, depth));

    // Top platform
    podium.addCube(glm::vec3(0.0f, height + 0.05f, 0.0f),
                   glm::vec3(width + 0.2f, 0.1f, depth + 0.2f));

    podium.setupFurniture();
    return podium;
}

Furniture Furniture::createBoard(float width, float height, float thickness)
{
    Furniture board;

    // Board surface
    board.addCube(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(width, height, thickness));

    // Frame
    float frameWidth = 0.1f;
    board.addCube(glm::vec3(0.0f, height / 2 + frameWidth / 2, 0.0f),
                  glm::vec3(width + frameWidth, frameWidth, thickness + 0.02f));
    board.addCube(glm::vec3(0.0f, -height / 2 - frameWidth / 2, 0.0f),
                  glm::vec3(width + frameWidth, frameWidth, thickness + 0.02f));
    board.addCube(glm::vec3(-width / 2 - frameWidth / 2, 0.0f, 0.0f),
                  glm::vec3(frameWidth, height, thickness + 0.02f));
    board.addCube(glm::vec3(width / 2 + frameWidth / 2, 0.0f, 0.0f),
                  glm::vec3(frameWidth, height, thickness + 0.02f));

    board.setupFurniture();
    return board;
}

void Furniture::setupFurniture()
{
    furnitureVAO.Bind();

    furnitureVBO = new VBO((GLfloat *)vertices.data(), vertices.size() * sizeof(FurnitureVertex));
    furnitureEBO = new EBO(indices.data(), indices.size() * sizeof(unsigned int));

    // Position attribute
    furnitureVAO.LinkVBOAttrib(*furnitureVBO, 0, 3, GL_FLOAT, sizeof(FurnitureVertex), (void *)0);
    // Normal attribute
    furnitureVAO.LinkVBOAttrib(*furnitureVBO, 1, 3, GL_FLOAT, sizeof(FurnitureVertex), (void *)(offsetof(FurnitureVertex, Normal)));
    // Texture coordinate attribute
    furnitureVAO.LinkVBOAttrib(*furnitureVBO, 2, 2, GL_FLOAT, sizeof(FurnitureVertex), (void *)(offsetof(FurnitureVertex, TexCoords)));

    furnitureVAO.Unbind();
    furnitureVBO->Unbind();
    furnitureEBO->Unbind();
}

void Furniture::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.Activate();

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    furnitureVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    furnitureVAO.Unbind();
}

void Furniture::Delete()
{
    furnitureVAO.Delete();
    if (furnitureVBO)
    {
        furnitureVBO->Delete();
        delete furnitureVBO;
    }
    if (furnitureEBO)
    {
        furnitureEBO->Delete();
        delete furnitureEBO;
    }
}