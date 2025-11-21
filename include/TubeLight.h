#ifndef TUBELIGHT_H
#define TUBELIGHT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

struct TubeLightVertex
{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Normal;
};

class TubeLight
{
public:
    std::vector<TubeLightVertex> vertices;
    std::vector<unsigned int> indices;
    VAO tubeVAO;
    Shader *emissiveShader;

    TubeLight(float roomLength, float roomWidth, float roomHeight);

    void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void Delete();

    // Get tube geometry for area lighting calculations
    glm::vec3 GetTubeCenter() const { return tubeCenter; }
    glm::vec3 GetTubeAxis() const { return tubeAxis; }
    float GetTubeLength() const { return tubeLength; }
    float GetTubeRadius() const { return tubeRadius; }

private:
    VBO *tubeVBO;
    EBO *tubeEBO;
    glm::vec3 tubeCenter;
    glm::vec3 tubeAxis; // Direction along tube (normalized)
    float tubeLength;
    float tubeRadius;

    void generateTubeLight(float roomLength, float roomWidth, float roomHeight);
    void setupTubeLight();
    void addTubeLightGeometry(glm::vec3 center, float tubeLength, float tubeRadius, float ceilingHeight);
};

#endif
