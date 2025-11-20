#ifndef LIGHTPANELS_H
#define LIGHTPANELS_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

struct LightVertex
{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Normal;
};

class LightPanels
{
public:
    std::vector<LightVertex> vertices;
    std::vector<unsigned int> indices;
    VAO lightVAO;
    Shader *emissiveShader; // Dedicated shader for glowing effect

    LightPanels(float roomLength, float roomWidth, float roomHeight, int rows, int cols,
                int light1Row, int light1Col, int light2Row, int light2Col);

    void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection); // No shader parameter
    void Delete();

private:
    VBO *lightVBO;
    EBO *lightEBO;

    void generateLightPanels(float roomLength, float roomWidth, float roomHeight, int rows, int cols,
                             int light1Row, int light1Col, int light2Row, int light2Col);
    void setupLightPanels();
    void addLightPanel(glm::vec3 center, float panelWidth, float panelHeight, float ceilingHeight);
};

#endif
