#ifndef FURNITURE_H
#define FURNITURE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

struct FurnitureVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Furniture
{
public:
    std::vector<FurnitureVertex> vertices;
    std::vector<unsigned int> indices;
    VAO furnitureVAO;

    static Furniture createDesk(float width, float height, float depth);
    static Furniture createChair(float width, float height, float depth);
    static Furniture createPodium(float width, float height, float depth);
    static Furniture createBoard(float width, float height, float thickness);

    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void Delete();

private:
    VBO *furnitureVBO;
    EBO *furnitureEBO;

    void setupFurniture();
    void addCube(glm::vec3 position, glm::vec3 size);
};

#endif