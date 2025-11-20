#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

class GreenBoard
{
public:
    GreenBoard(float roomLength, float roomWidth, float roomHeight);

    // Draw green boards with metallic frames
    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void Delete();

private:
    VAO boardVAO;
    VBO *boardVBO;
    EBO *boardEBO;
    unsigned int numBoardIndices;

    VAO frameVAO;
    VBO *frameVBO;
    EBO *frameEBO;
    unsigned int numFrameIndices;

    float boardWidth;
    float boardHeight;
    float frameThickness;
};
