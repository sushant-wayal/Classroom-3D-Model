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

class Door
{
public:
    Door(float roomLength, float roomWidth, float roomHeight);

    // Draw door with frame and panel
    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void Delete();

private:
    VAO doorVAO;
    VBO *doorVBO;
    EBO *doorEBO;
    unsigned int numDoorIndices;

    VAO frameVAO;
    VBO *frameVBO;
    EBO *frameEBO;
    unsigned int numFrameIndices;

    float doorWidth;
    float doorHeight;
    float frameThickness;
};
