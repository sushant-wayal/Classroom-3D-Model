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

class ProjectorScreen
{
public:
    ProjectorScreen(float roomLength, float roomWidth, float roomHeight);
    void Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void Update(float deltaTime); 
    void ToggleScreen();          
    void Delete();

    bool isDroppedDown;
    float screenExtension; 

private:
    VAO screenVAO;
    VBO *screenVBO;
    EBO *screenEBO;

    unsigned int numScreenIndices;

    float screenWidth;     
    float screenMaxHeight; 
    float rodY;           
    float screenZ;         
    float borderWidth;     
    float animationSpeed;  
    float targetExtension; 

    void UpdateScreenGeometry(); 
};
