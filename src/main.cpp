#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "constants.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Furniture.h"
#include "CeilingTiles.h"
#include "Windows.h"
#include "RightWallWindows.h"
#include "GreenBoard.h"
#include "models/Model.h"

glm::vec3 cameraPos = glm::vec3(-10.0f, 3.0f, 2.0f); // Left side view position
glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f); // Looking right towards the classroom
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = 0.0f;   // Facing right (positive X direction)
float pitch = 0.0f; // Level horizon
float lastX = window::width / 2.0f;
float lastY = window::height / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos = glm::vec3(0.0f, 6.0f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);

float fanRotationSpeed[furniture::fans];

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 8.0f * deltaTime; // camera speed
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;

    // Camera preset positions
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        // Left side view
        cameraPos = glm::vec3(-10.0f, 3.0f, 2.0f);
        cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
        yaw = 0.0f;
        pitch = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        // Right side view
        cameraPos = glm::vec3(10.0f, 3.0f, 2.0f);
        cameraFront = glm::vec3(-1.0f, 0.0f, 0.0f);
        yaw = 180.0f;
        pitch = 0.0f;
    }

    // Reset camera to left side view (which works well)
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        cameraPos = glm::vec3(-10.0f, 3.0f, 2.0f);
        cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
        yaw = 0.0f;
        pitch = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
        fanRotationSpeed[0] = 2.0f; // Resume rotation
    }

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
    {
        fanRotationSpeed[1] = 2.0f; // Resume rotation
    }

    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
    {
        fanRotationSpeed[0] = 0.0f; // Stop rotation
    }

    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
    {
        fanRotationSpeed[1] = 0.0f; // Stop rotation
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Keep only essential anti-aliasing for smooth edges
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(
        window::width, window::height, window::title, NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE);

    for (int i = 0; i < furniture::fans; i++)
    {
        fanRotationSpeed[i] = 2.0f;
    }

    // Classroom dimensions (in meters, scaled for OpenGL)
    float roomLength = room::length;
    float roomWidth = room::width;
    float roomHeight = room::height;

    // Calculate window parameters - MUST MATCH Windows.cpp exactly
    float frameThicknessCalc = 0.05f;            // 5cm frame thickness (THINNER!)
    float windowHeightCalc = roomHeight * 0.20f; // 20% height - SHORTER
    float topMargin = 0.30f;                     // 30cm from ceiling (REDUCED from 60cm)
    float windowTopY = roomHeight - topMargin;
    float windowBottomY = windowTopY - windowHeightCalc;

    float sideMargin = frameThicknessCalc + 0.05f; // 10cm margin on each side (frames stay inside)
    float spacingBetweenWindows = 0.50f;           // 50cm gap between windows
    float totalAvailableLength = roomLength - (2.0f * sideMargin);
    float totalSpacing = spacingBetweenWindows * (8 - 1); // 8 windows
    float windowWidthCalc = (totalAvailableLength - totalSpacing) / 8;
    float startX = -roomLength / 2.0f + sideMargin;

    // Create back wall vertices with 8 INDIVIDUAL window holes
    std::vector<GLfloat> backWallVertices;
    std::vector<GLuint> backWallIndices;

    float backWallZ = -roomWidth / 2.0f;
    float leftWallX = -roomLength / 2.0f;
    float rightWallX = roomLength / 2.0f;

    // Colors for wall
    float wallR = 0.95f, wallG = 0.90f, wallB = 0.70f;

    unsigned int vertexIndex = 0;

    // Bottom section (floor to window bottom) - SOLID WALL
    backWallVertices.insert(backWallVertices.end(), {leftWallX, 0.0f, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                     rightWallX, 0.0f, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                     rightWallX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                     leftWallX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f});
    backWallIndices.insert(backWallIndices.end(), {0, 1, 2, 2, 3, 0});
    vertexIndex = 4;

    // Top section (window top to ceiling) - SOLID WALL
    backWallVertices.insert(backWallVertices.end(), {leftWallX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                     rightWallX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                     rightWallX, roomHeight, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                     leftWallX, roomHeight, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f});
    backWallIndices.insert(backWallIndices.end(), {vertexIndex + 0, vertexIndex + 1, vertexIndex + 2,
                                                   vertexIndex + 2, vertexIndex + 3, vertexIndex + 0});
    vertexIndex += 4;

    // Middle section - wall with 8 INDIVIDUAL window holes
    // Create wall sections between and around windows
    for (int i = 0; i < 8; i++)
    {
        float windowLeftX = startX + i * (windowWidthCalc + spacingBetweenWindows);
        float windowRightX = windowLeftX + windowWidthCalc;

        if (i == 0)
        {
            // Left edge wall section (from left wall to first window)
            backWallVertices.insert(backWallVertices.end(), {leftWallX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             windowLeftX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             windowLeftX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             leftWallX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f});
            backWallIndices.insert(backWallIndices.end(), {vertexIndex + 0, vertexIndex + 1, vertexIndex + 2,
                                                           vertexIndex + 2, vertexIndex + 3, vertexIndex + 0});
            vertexIndex += 4;
        }

        if (i < 7)
        {
            // Wall section BETWEEN this window and next window
            float nextWindowLeftX = startX + (i + 1) * (windowWidthCalc + spacingBetweenWindows);

            backWallVertices.insert(backWallVertices.end(), {windowRightX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             nextWindowLeftX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             nextWindowLeftX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             windowRightX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f});
            backWallIndices.insert(backWallIndices.end(), {vertexIndex + 0, vertexIndex + 1, vertexIndex + 2,
                                                           vertexIndex + 2, vertexIndex + 3, vertexIndex + 0});
            vertexIndex += 4;
        }

        if (i == 7)
        {
            // Right edge wall section (from last window to right wall)
            backWallVertices.insert(backWallVertices.end(), {windowRightX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             rightWallX, windowBottomY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             rightWallX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f,
                                                             windowRightX, windowTopY, backWallZ, wallR, wallG, wallB, 0.0f, 0.0f, 1.0f});
            backWallIndices.insert(backWallIndices.end(), {vertexIndex + 0, vertexIndex + 1, vertexIndex + 2,
                                                           vertexIndex + 2, vertexIndex + 3, vertexIndex + 0});
            vertexIndex += 4;
        }
    }

    // Vertices for classroom walls, floor, and ceiling with colors and normals
    // Format: x, y, z, r, g, b, nx, ny, nz
    GLfloat vertices[] = {
        // Floor (y = 0) - Shiny white tile
        -roomLength / 2, 0.0f, -roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,
        roomLength / 2, 0.0f, -roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,
        roomLength / 2, 0.0f, roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,
        -roomLength / 2, 0.0f, roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,

        // Front wall (z = roomWidth/2)
        -roomLength / 2, 0.0f, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,
        roomLength / 2, 0.0f, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,
        roomLength / 2, roomHeight, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,
        -roomLength / 2, roomHeight, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,

        // Left wall (x = -roomLength/2)
        -roomLength / 2, 0.0f, -roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, 0.0f, roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, roomHeight, roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, roomHeight, -roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f};

    GLuint indices[] = {
        // Floor
        0, 1, 2, 2, 3, 0,
        // Front wall
        4, 6, 5, 6, 4, 7,
        // Left wall
        8, 10, 9, 10, 8, 11};
    // Right wall REMOVED - created separately with window holes

    // Create shader program for basic room
    Shader roomShader("shaders/default.vert", "shaders/default.frag");

    // Create shader program for furniture with lighting
    Shader furnitureShader("shaders/texture.vert", "shaders/texture.frag");

    // Create vertex array and buffers for room (floor and side walls)
    VAO roomVAO;
    roomVAO.Bind();

    VBO roomVBO(vertices, sizeof(vertices));
    EBO roomEBO(indices, sizeof(indices));

    roomVAO.LinkVBOAttrib(roomVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    roomVAO.LinkVBOAttrib(roomVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    roomVAO.LinkVBOAttrib(roomVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));

    roomVAO.Unbind();
    roomVBO.Unbind();
    roomEBO.Unbind();

    // Create VAO/VBO/EBO for back wall with 8 individual holes
    VAO backWallVAO;
    backWallVAO.Bind();

    VBO backWallVBO(backWallVertices.data(), backWallVertices.size() * sizeof(GLfloat));
    EBO backWallEBO(backWallIndices.data(), backWallIndices.size() * sizeof(GLuint));

    backWallVAO.LinkVBOAttrib(backWallVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    backWallVAO.LinkVBOAttrib(backWallVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    backWallVAO.LinkVBOAttrib(backWallVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));

    backWallVAO.Unbind();
    backWallVBO.Unbind();
    backWallEBO.Unbind();

    // Create right wall vertices with 6 INDIVIDUAL window holes (5 back + 1 front)
    std::vector<GLfloat> rightWallVertices;
    std::vector<GLuint> rightWallIndices;

    unsigned int rightVertexIndex = 0;
    float frontWallZ = roomWidth / 2.0f;

    // Right wall window parameters - same as back wall
    float rightWindowWidth = windowWidthCalc; // Same width as back wall windows (2.7875m)
    float backMarginRight = 0.5f;
    float frontMarginRight = 0.5f;

    // BACK SECTION: 5 windows
    int numBackWindows = 5;
    float backSectionStart = backWallZ + backMarginRight;
    float backSectionLength = numBackWindows * rightWindowWidth + (numBackWindows - 1) * spacingBetweenWindows;

    // FRONT SECTION: 1 window (CHANGED from 2 to 1)
    int numFrontWindows = 1;
    float frontSectionEnd = frontWallZ - frontMarginRight;
    float frontSectionLength = numFrontWindows * rightWindowWidth; // No spacing needed for single window
    float frontSectionStart = frontSectionEnd - frontSectionLength;

    // Bottom section (floor to window bottom) - SOLID WALL
    rightWallVertices.insert(rightWallVertices.end(), {rightWallX, 0.0f, backWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, 0.0f, frontWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, windowBottomY, frontWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, windowBottomY, backWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f});
    rightWallIndices.insert(rightWallIndices.end(), {0, 1, 2, 2, 3, 0});
    rightVertexIndex = 4;

    // Top section (window top to ceiling) - SOLID WALL
    rightWallVertices.insert(rightWallVertices.end(), {rightWallX, windowTopY, backWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, windowTopY, frontWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, roomHeight, frontWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, roomHeight, backWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f});
    rightWallIndices.insert(rightWallIndices.end(), {rightVertexIndex + 0, rightVertexIndex + 1, rightVertexIndex + 2,
                                                     rightVertexIndex + 2, rightVertexIndex + 3, rightVertexIndex + 0});
    rightVertexIndex += 4;

    // Middle section - wall with 6 INDIVIDUAL window holes in 2 sections
    // BACK SECTION: Create wall sections for 5 windows
    for (int i = 0; i < numBackWindows; i++)
    {
        float windowBackZ = backSectionStart + i * (rightWindowWidth + spacingBetweenWindows);
        float windowFrontZ = windowBackZ + rightWindowWidth;

        if (i == 0)
        {
            // Back edge wall section (from back wall to first window)
            rightWallVertices.insert(rightWallVertices.end(), {rightWallX, windowBottomY, backWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowBottomY, windowBackZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowTopY, windowBackZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowTopY, backWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f});
            rightWallIndices.insert(rightWallIndices.end(), {rightVertexIndex + 0, rightVertexIndex + 1, rightVertexIndex + 2,
                                                             rightVertexIndex + 2, rightVertexIndex + 3, rightVertexIndex + 0});
            rightVertexIndex += 4;
        }

        if (i < numBackWindows - 1)
        {
            // Wall section BETWEEN this window and next window in back section
            float nextWindowBackZ = backSectionStart + (i + 1) * (rightWindowWidth + spacingBetweenWindows);

            rightWallVertices.insert(rightWallVertices.end(), {rightWallX, windowBottomY, windowFrontZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowBottomY, nextWindowBackZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowTopY, nextWindowBackZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowTopY, windowFrontZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f});
            rightWallIndices.insert(rightWallIndices.end(), {rightVertexIndex + 0, rightVertexIndex + 1, rightVertexIndex + 2,
                                                             rightVertexIndex + 2, rightVertexIndex + 3, rightVertexIndex + 0});
            rightVertexIndex += 4;
        }

        if (i == numBackWindows - 1)
        {
            // Large GAP section between back section and front section
            rightWallVertices.insert(rightWallVertices.end(), {rightWallX, windowBottomY, windowFrontZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowBottomY, frontSectionStart, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowTopY, frontSectionStart, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                               rightWallX, windowTopY, windowFrontZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f});
            rightWallIndices.insert(rightWallIndices.end(), {rightVertexIndex + 0, rightVertexIndex + 1, rightVertexIndex + 2,
                                                             rightVertexIndex + 2, rightVertexIndex + 3, rightVertexIndex + 0});
            rightVertexIndex += 4;
        }
    }

    // FRONT SECTION: Create wall section for 1 window
    // Front edge wall section (from last window to front wall)
    float windowBackZ = frontSectionStart;
    float windowFrontZ = windowBackZ + rightWindowWidth;

    rightWallVertices.insert(rightWallVertices.end(), {rightWallX, windowBottomY, windowFrontZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, windowBottomY, frontWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, windowTopY, frontWallZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f,
                                                       rightWallX, windowTopY, windowFrontZ, wallR, wallG, wallB, -1.0f, 0.0f, 0.0f});
    rightWallIndices.insert(rightWallIndices.end(), {rightVertexIndex + 0, rightVertexIndex + 1, rightVertexIndex + 2,
                                                     rightVertexIndex + 2, rightVertexIndex + 3, rightVertexIndex + 0});
    rightVertexIndex += 4;

    // Create VAO/VBO/EBO for right wall with 6 individual holes
    VAO rightWallVAO;
    rightWallVAO.Bind();

    VBO rightWallVBO(rightWallVertices.data(), rightWallVertices.size() * sizeof(GLfloat));
    EBO rightWallEBO(rightWallIndices.data(), rightWallIndices.size() * sizeof(GLuint));

    rightWallVAO.LinkVBOAttrib(rightWallVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    rightWallVAO.LinkVBOAttrib(rightWallVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    rightWallVAO.LinkVBOAttrib(rightWallVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));

    rightWallVAO.Unbind();
    rightWallVBO.Unbind();
    rightWallEBO.Unbind();

    // Create furniture pieces - Simple texture application
    std::cout << "Loading custom Blender models..." << std::endl;

    // Load custom desk model with simple texture application
    Model customDesk("models/classroom_desk.obj", "textures/desk_texture.jpeg");
    std::cout << "Custom desk model loaded with wood texture applied to entire model!" << std::endl;

    Model customFan("models/classroom_fan.obj");
    std::cout << "Custom Fan Model loaded successfully!" << std::endl;

    // Load custom podium model
    Model customPodium("models/classroom_podium.obj");
    std::cout << "Custom Podium Model loaded successfully!" << std::endl;

    // Create realistic tiled ceiling with adjusted dimensions for classroom
    std::cout << "Creating tiled ceiling..." << std::endl;
    CeilingTiles ceilingTiles(roomLength, roomWidth, roomHeight, 10, 15); // 10 rows x 15 columns with larger tiles for 32m x 21.5m room
    std::cout << "Tiled ceiling created successfully!" << std::endl;

    // Create windows on back wall with transparent glass and white plastic frames
    std::cout << "Creating back wall windows..." << std::endl;
    Windows backWallWindows(roomLength, roomWidth, roomHeight, 8); // 8 windows on back wall
    std::cout << "Back wall windows created successfully!" << std::endl;

    // Create windows on right wall with 6 windows (5 back + 1 front sections)
    std::cout << "Creating right wall windows..." << std::endl;
    RightWallWindows rightWallWindows(roomLength, roomWidth, roomHeight); // 6 windows on right wall
    std::cout << "Right wall windows created successfully!" << std::endl;

    // Create green boards on front wall
    std::cout << "Creating green boards on front wall..." << std::endl;
    GreenBoard greenBoards(roomLength, roomWidth, roomHeight);
    std::cout << "Green boards created successfully!" << std::endl;

    std::cout << "All furniture loaded successfully!" << std::endl;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Render - Sky blue background (visible through transparent windows)
        glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // Beautiful light sky blue color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create transformations
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f),
                                                (float)window::width / (float)window::height,
                                                0.1f, 100.0f);

        // Render room (walls, floor, ceiling)
        roomShader.Activate();
        glm::mat4 roomModel = glm::mat4(1.0f);

        glUniformMatrix4fv(glGetUniformLocation(roomShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(roomModel));
        glUniformMatrix4fv(glGetUniformLocation(roomShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(roomShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Set lighting uniforms for room
        glUniform3fv(glGetUniformLocation(roomShader.ID, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(roomShader.ID, "lightColor"), 1, glm::value_ptr(lightColor));

        roomVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        // Render back wall with 8 individual holes
        backWallVAO.Bind();
        glDrawElements(GL_TRIANGLES, backWallIndices.size(), GL_UNSIGNED_INT, 0);

        // Render right wall with 6 individual holes (5 back + 1 front)
        rightWallVAO.Bind();
        glDrawElements(GL_TRIANGLES, rightWallIndices.size(), GL_UNSIGNED_INT, 0);

        // Render tiled ceiling
        ceilingTiles.Draw(roomShader, roomModel, view, projection);

        // Render back wall windows with transparent glass and white frames
        backWallWindows.Draw(roomShader, roomModel, view, projection);

        // Render right wall windows with transparent glass and white frames
        rightWallWindows.Draw(roomShader, roomModel, view, projection);

        // Render green boards on front wall
        greenBoards.Draw(roomShader, roomModel, view, projection);

        // Render furniture with lighting
        furnitureShader.Activate();

        // Set lighting uniforms
        glUniform3fv(glGetUniformLocation(furnitureShader.ID, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(furnitureShader.ID, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(furnitureShader.ID, "viewPos"), 1, glm::value_ptr(cameraPos));

        // Scale factor to make desk proportional to classroom size
        float deskScale = furniture::deskScale;
        int noOfRows = furniture::rows;
        int noOfCols = furniture::cols;

        float deskYPos = 0.0f;

        // FIXED spacing between benches - NO OVERLAP!
        // Each scaled bench is approximately 1.8m wide and 1.6m deep (seat + backrest = 2m total)
        float spacingBetweenCols = 5.0f; // SIGNIFICANTLY INCREASED to 5.0m spacing between column centers (MAXIMUM SEPARATION!)
        float spacingBetweenRows = 3.5f; // Fixed 3.5m spacing between row centers (PERFECT - DO NOT CHANGE!)

        // Actual bench dimensions (after scaling)
        float benchWidth = 1.8f; // Actual width of each bench

        // Calculate total space needed for benches
        float totalBenchWidth = (noOfCols - 1) * spacingBetweenCols;
        float totalBenchDepth = (noOfRows - 1) * spacingBetweenRows;

        // Calculate total width occupied including the actual physical extent of benches
        // The grid spans from leftmost bench edge to rightmost bench edge
        float totalOccupiedWidth = totalBenchWidth + benchWidth;

        // Back margin from back wall (extra space for backrest of last row)
        float backMargin = 2.5f; // 2.5m to ensure backrests don't clip through wall

        // Calculate starting positions to center benches with equal side spacing
        // Z-axis: Start from back, leaving back margin, benches go towards front
        float startZ = -roomWidth / 2 + backMargin;

        // X-axis: Center the benches horizontally with equal side space
        // Total occupied width includes spacing between benches PLUS the physical width of benches
        float totalGridWidth = (noOfCols - 1) * spacingBetweenCols + benchWidth;
        float sideSpace = (roomLength - totalGridWidth) / 2.0f;
        // Start at left wall + side space + half bench width (since benches are placed at their centers)
        float startX = -roomLength / 2 + sideSpace + (benchWidth / 2.0f);

        for (int row = 0; row < noOfRows; row++)
        {
            for (int col = 0; col < noOfCols; col++)
            {
                glm::mat4 deskModel = glm::mat4(1.0f);

                float x = startX + col * spacingBetweenCols;
                float z = startZ + row * spacingBetweenRows;

                deskModel = glm::translate(deskModel, glm::vec3(x, deskYPos, z));
                deskModel = glm::scale(deskModel, glm::vec3(deskScale, deskScale, deskScale));
                // Rotate 180 degrees around Y-axis to face the blackboard (front of classroom)
                deskModel = glm::rotate(deskModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

                customDesk.Draw(furnitureShader, deskModel, view, projection);
            }
        }

        // Debug output to verify layout
        if (glfwGetTime() < 1.0f)
        { // Print once at startup
            std::cout << "\n=== BENCH LAYOUT DEBUG ===" << std::endl;
            std::cout << "Room Width (Z-axis): " << roomWidth << "m" << std::endl;
            std::cout << "Back margin (for backrest): " << backMargin << "m" << std::endl;
            std::cout << "ROW SPACING: " << spacingBetweenRows << "m (PERFECT - DO NOT CHANGE!)" << std::endl;
            std::cout << "COLUMN SPACING: " << spacingBetweenCols << "m (SIGNIFICANTLY INCREASED!)" << std::endl;
            std::cout << "Total bench grid width: " << totalBenchWidth << "m" << std::endl;
            std::cout << "Total bench grid depth: " << totalBenchDepth << "m" << std::endl;
            std::cout << "Benches start X: " << startX << "m (leftmost)" << std::endl;
            std::cout << "Benches end X: " << (startX + totalBenchWidth) << "m (rightmost)" << std::endl;
            std::cout << "Left wall at X: " << (-roomLength / 2) << "m" << std::endl;
            std::cout << "Right wall at X: " << (roomLength / 2) << "m" << std::endl;
            std::cout << "Benches start Z: " << startZ << "m" << std::endl;
            std::cout << "Benches end Z (last row center): " << (startZ + totalBenchDepth) << "m" << std::endl;
            std::cout << "Back wall at Z: " << (-roomWidth / 2) << "m" << std::endl;
            std::cout << "Front wall at Z: " << (roomWidth / 2) << "m" << std::endl;
            std::cout << "Front space: " << (roomWidth / 2 - (startZ + totalBenchDepth) - 1.0f) << "m" << std::endl;
            std::cout << "========================\n"
                      << std::endl;
        }

        float fanScale = furniture::fanScale;
        int noOfFans = furniture::fans;
        int fanRows = furniture::fanRows;
        int fanCols = furniture::fanCols;

        float fanYPos = roomHeight - 1.2f;

        // Calculate spacing for 2x3 fan grid
        float fanSpacingX = roomLength * 0.7f / (fanCols - 1); // Use 70% of room length
        float fanSpacingZ = roomWidth * 0.6f / (fanRows - 1);  // Use 60% of room width
        float fanStartX = -(roomLength * 0.35f);               // Center the grid
        float fanStartZ = -(roomWidth * 0.3f);                 // Center the grid

        float fanRotation[noOfFans];
        for (int i = 0; i < noOfFans; i++)
        {
            fanRotation[i] = fmod(glfwGetTime() * fanRotationSpeed[i] * 360.0f + i * 45.0f, 360.0f);
        }

        int fanIndex = 0;
        for (int row = 0; row < fanRows; row++)
        {
            for (int col = 0; col < fanCols; col++)
            {
                glm::mat4 fanModel = glm::mat4(1.0f);

                float x = fanStartX + col * fanSpacingX;
                float z = fanStartZ + row * fanSpacingZ;

                fanModel = glm::translate(fanModel, glm::vec3(x, fanYPos, z));
                fanModel = glm::scale(fanModel, glm::vec3(fanScale, fanScale, fanScale));
                // Add rotation around Y-axis (vertical) for fan blades
                fanModel = glm::rotate(fanModel, glm::radians(fanRotation[fanIndex]), glm::vec3(0.0f, 1.0f, 0.0f));

                customFan.Draw(furnitureShader, fanModel, view, projection);
                fanIndex++;
            }
        }

        // Render podium in front of class near the right side
        glm::mat4 podiumModel = glm::mat4(1.0f);

        // Position the podium in front of the class, near the right side
        // X position: towards the right side of the room (positive X)
        // Y position: adjusted to just touch the floor (not floating, not below)
        // Z position: in the front area (positive Z, near front wall)
        float podiumX = roomLength / 2 - 5.5f; // 5.5m from right wall
        float podiumY = 1.7f;                  // Lowered to 1.7m - should just touch the floor perfectly
        float podiumZ = roomWidth / 2 - 2.0f;  // 2m from front wall
        float podiumScale = 1.5f;              // Scale to appropriate size for classroom

        podiumModel = glm::translate(podiumModel, glm::vec3(podiumX, podiumY, podiumZ));
        podiumModel = glm::scale(podiumModel, glm::vec3(podiumScale, podiumScale, podiumScale));
        // Rotate 90 degrees (positive) to face the back wall (negative Z direction where benches are)
        podiumModel = glm::rotate(podiumModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        customPodium.Draw(furnitureShader, podiumModel, view, projection);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    roomVAO.Delete();
    roomVBO.Delete();
    roomEBO.Delete();
    backWallVAO.Delete();
    backWallVBO.Delete();
    backWallEBO.Delete();
    rightWallVAO.Delete();
    rightWallVBO.Delete();
    rightWallEBO.Delete();
    roomShader.Delete();
    furnitureShader.Delete();

    // Clean up furniture
    customDesk.Delete();
    customFan.Delete();
    customPodium.Delete();
    ceilingTiles.Delete();
    backWallWindows.Delete();
    rightWallWindows.Delete();
    greenBoards.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}