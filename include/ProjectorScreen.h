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
    void Update(float deltaTime); // Update animation
    void ToggleScreen();          // Toggle between rolled up and dropped down
    void Delete();

    // Animation state
    bool isDroppedDown;
    float screenExtension; // How far the screen has extended (0.0 to 1.0)

private:
    VAO screenVAO;
    VBO *screenVBO;
    EBO *screenEBO;

    unsigned int numScreenIndices;

    float screenWidth;     // Width of the screen
    float screenMaxHeight; // Maximum height when fully dropped
    float rodY;            // Y position of the rod
    float screenZ;         // Z position on front wall
    float borderWidth;     // Width of black borders on left, right, and bottom

    // Animation parameters
    float animationSpeed;  // Speed of animation
    float targetExtension; // Target extension (0.0 or 1.0)

    void UpdateScreenGeometry(); // Regenerate screen geometry based on current extension
};
