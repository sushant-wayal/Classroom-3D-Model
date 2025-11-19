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

class Windows
{
public:
    Windows(float roomLength, float roomWidth, float roomHeight, int numWindows = 8);

    // Draw windows with frames and glass
    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);

    void Delete();

private:
    VAO glassVAO;
    VBO *glassVBO;
    EBO *glassEBO;
    unsigned int numGlassIndices;

    VAO frameVAO;
    VBO *frameVBO;
    EBO *frameEBO;
    unsigned int numFrameIndices;

    float windowWidth;
    float windowHeight;
    float frameThickness;
    int numWindows;
};
