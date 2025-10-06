#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "shaderClass.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Model
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    VAO modelVAO;

    Model(const char *objFile);

    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void Delete();

private:
    VBO *modelVBO;
    EBO *modelEBO;

    void loadOBJ(const char *objFile);
    void setupModel();
};

#endif