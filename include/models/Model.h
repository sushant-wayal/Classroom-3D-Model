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
#include <map>

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
    void generateSmoothNormals();
    void processVertex(const std::string &vertexStr, const std::vector<glm::vec3> &temp_vertices, const std::vector<glm::vec2> &temp_uvs, const std::vector<glm::vec3> &temp_normals, std::map<std::string, unsigned int> &vertexMap);
};

#endif