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

struct Material
{
    std::string name;
    glm::vec3 ambient;   // Ka
    glm::vec3 diffuse;   // Kd
    glm::vec3 specular;  // Ks
    float shininess;     // Ns
    Texture *diffuseMap; // map_Kd

    Material() : ambient(1.0f), diffuse(0.8f), specular(0.5f), shininess(32.0f), diffuseMap(nullptr) {}
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material *material;

    VAO meshVAO;
    VBO *meshVBO;
    EBO *meshEBO;

    Mesh() : material(nullptr), meshVBO(nullptr), meshEBO(nullptr) {}

    void setupMesh();
    void Draw(Shader &shader);
    void Delete();
};

class Model
{
public:
    std::vector<Mesh> meshes;
    std::map<std::string, Material *> materials;

    Model(const char *objFile);
    Model(const char *objFile, const char *texturePath);

    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void Delete();

private:
    void loadOBJ(const char *objFile);
    void loadMTL(const std::string &mtlFile, const std::string &basePath);
    void processVertex(const std::string &vertexStr, const std::vector<glm::vec3> &temp_vertices,
                       const std::vector<glm::vec2> &temp_uvs, const std::vector<glm::vec3> &temp_normals,
                       std::map<std::string, unsigned int> &vertexMap, Mesh &currentMesh);
};

#endif