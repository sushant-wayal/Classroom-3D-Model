#ifndef CEILINGTILES_H
#define CEILINGTILES_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

struct CeilingVertex
{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Normal;
};

class CeilingTiles
{
public:
    std::vector<CeilingVertex> vertices;
    std::vector<unsigned int> indices;
    VAO ceilingVAO;

    CeilingTiles(float roomLength, float roomWidth, float roomHeight, int rows, int cols);

    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void Delete();

private:
    VBO *ceilingVBO;
    EBO *ceilingEBO;

    void generateCeilingTiles(float roomLength, float roomWidth, float roomHeight, int rows, int cols);
    void setupCeiling();
    void addTile(glm::vec3 center, float tileWidth, float tileHeight, float ceilingHeight, float borderWidth, float extrusionDepth);
};

#endif