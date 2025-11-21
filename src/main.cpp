#include <iostream>
#include <vector>
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
#include "LightPanels.h"
#include "TubeLight.h"
#include "Windows.h"
#include "RightWallWindows.h"
#include "GreenBoard.h"
#include "Door.h"
#include "ProjectorScreen.h"
#include "models/Model.h"

// Camera state
glm::vec3 cameraPos = glm::vec3(-10.0f, 3.0f, 2.0f);
glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = 0.0f, pitch = 0.0f;
float lastX = window::width / 2.0f;
float lastY = window::height / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f, lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos1, lightPos2;
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);

float fanRotationSpeed[furniture::fans];
ProjectorScreen *projectorScreen = nullptr;

void setCameraPreset(int preset)
{
    if (preset == 1)
    {
        cameraPos = glm::vec3(-10.0f, 3.0f, 2.0f);
        cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
        yaw = pitch = 0.0f;
    }
    else if (preset == 2)
    {
        cameraPos = glm::vec3(10.0f, 3.0f, 2.0f);
        cameraFront = glm::vec3(-1.0f, 0.0f, 0.0f);
        yaw = 180.0f;
        pitch = 0.0f;
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 8.0f * deltaTime;
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

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        setCameraPreset(1);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        setCameraPreset(2);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        setCameraPreset(1);

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        fanRotationSpeed[0] = 2.0f;
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
        fanRotationSpeed[1] = 2.0f;
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
        fanRotationSpeed[0] = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
        fanRotationSpeed[1] = 0.0f;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_P && projectorScreen)
        projectorScreen->ToggleScreen();
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = (xpos - lastX) * 0.1f;
    float yoffset = (lastY - ypos) * 0.1f;
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset;
    pitch = glm::clamp(pitch + yoffset, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void addWallQuad(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices,
                 unsigned int &idx, float x1, float y1, float z1, float x2, float y2, float z2,
                 float r, float g, float b, float nx, float ny, float nz)
{
    vertices.insert(vertices.end(), {x1, y1, z1, r, g, b, nx, ny, nz,
                                     x2, y1, z2, r, g, b, nx, ny, nz,
                                     x2, y2, z2, r, g, b, nx, ny, nz,
                                     x1, y2, z1, r, g, b, nx, ny, nz});
    indices.insert(indices.end(), {idx, idx + 1, idx + 2, idx + 2, idx + 3, idx});
    idx += 4;
}

void createBackWall(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices,
                    float roomLength, float roomWidth, float roomHeight,
                    float windowBottomY, float windowTopY, float windowWidth,
                    float startX, float spacing, int numWindows)
{
    float backWallZ = -roomWidth / 2.0f;
    float leftX = -roomLength / 2.0f, rightX = roomLength / 2.0f;
    float wallR = 0.95f, wallG = 0.90f, wallB = 0.70f;
    unsigned int idx = 0;

    addWallQuad(vertices, indices, idx, leftX, 0.0f, backWallZ, rightX, windowBottomY, backWallZ,
                wallR, wallG, wallB, 0.0f, 0.0f, 1.0f);
    addWallQuad(vertices, indices, idx, leftX, windowTopY, backWallZ, rightX, roomHeight, backWallZ,
                wallR, wallG, wallB, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < numWindows; i++)
    {
        float winLeft = startX + i * (windowWidth + spacing);
        float winRight = winLeft + windowWidth;

        if (i == 0)
            addWallQuad(vertices, indices, idx, leftX, windowBottomY, backWallZ, winLeft, windowTopY, backWallZ,
                        wallR, wallG, wallB, 0.0f, 0.0f, 1.0f);

        if (i < numWindows - 1)
        {
            float nextWinLeft = startX + (i + 1) * (windowWidth + spacing);
            addWallQuad(vertices, indices, idx, winRight, windowBottomY, backWallZ, nextWinLeft, windowTopY, backWallZ,
                        wallR, wallG, wallB, 0.0f, 0.0f, 1.0f);
        }

        if (i == numWindows - 1)
            addWallQuad(vertices, indices, idx, winRight, windowBottomY, backWallZ, rightX, windowTopY, backWallZ,
                        wallR, wallG, wallB, 0.0f, 0.0f, 1.0f);
    }
}

void createRightWall(std::vector<GLfloat> &vertices, std::vector<GLuint> &indices,
                     float roomLength, float roomWidth, float roomHeight,
                     float windowBottomY, float windowTopY, float rightWindowWidth,
                     float backMargin, float frontMargin, float spacing, float doorBackZ,
                     float doorFrontZ, float doorTopY)
{
    float rightX = roomLength / 2.0f;
    float backWallZ = -roomWidth / 2.0f, frontWallZ = roomWidth / 2.0f;
    float wallR = 0.95f, wallG = 0.90f, wallB = 0.70f;
    unsigned int idx = 0;

    // Bottom sections with door hole
    addWallQuad(vertices, indices, idx, rightX, 0.0f, backWallZ, rightX, windowBottomY, doorBackZ,
                wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);

    if (doorTopY < windowBottomY)
        addWallQuad(vertices, indices, idx, rightX, doorTopY, doorBackZ, rightX, windowBottomY, doorFrontZ,
                    wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);

    addWallQuad(vertices, indices, idx, rightX, 0.0f, doorFrontZ, rightX, windowBottomY, frontWallZ,
                wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);

    // Top section
    addWallQuad(vertices, indices, idx, rightX, windowTopY, backWallZ, rightX, roomHeight, frontWallZ,
                wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);

    // Window sections
    int numBackWindows = 5, numFrontWindows = 1;
    float backSectionStart = backWallZ + backMargin;
    float frontSectionStart = frontWallZ - frontMargin - rightWindowWidth;

    for (int i = 0; i < numBackWindows; i++)
    {
        float winBack = backSectionStart + i * (rightWindowWidth + spacing);
        float winFront = winBack + rightWindowWidth;

        if (i == 0)
            addWallQuad(vertices, indices, idx, rightX, windowBottomY, backWallZ, rightX, windowTopY, winBack,
                        wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);

        if (i < numBackWindows - 1)
        {
            float nextWinBack = backSectionStart + (i + 1) * (rightWindowWidth + spacing);
            addWallQuad(vertices, indices, idx, rightX, windowBottomY, winFront, rightX, windowTopY, nextWinBack,
                        wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);
        }

        if (i == numBackWindows - 1)
            addWallQuad(vertices, indices, idx, rightX, windowBottomY, winFront, rightX, windowTopY, frontSectionStart,
                        wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);
    }

    float winFront = frontSectionStart + rightWindowWidth;
    addWallQuad(vertices, indices, idx, rightX, windowBottomY, winFront, rightX, windowTopY, frontWallZ,
                wallR, wallG, wallB, -1.0f, 0.0f, 0.0f);
}

void setLightingUniforms(GLuint shaderID, const glm::vec3 &light1, const glm::vec3 &light2,
                         const TubeLight &tubeLight, const glm::vec3 &color, const glm::vec3 *viewPos = nullptr)
{
    glUniform3fv(glGetUniformLocation(shaderID, "lightPos1"), 1, glm::value_ptr(light1));
    glUniform3fv(glGetUniformLocation(shaderID, "lightPos2"), 1, glm::value_ptr(light2));
    glUniform3fv(glGetUniformLocation(shaderID, "tubeCenter"), 1, glm::value_ptr(tubeLight.GetTubeCenter()));
    glUniform3fv(glGetUniformLocation(shaderID, "tubeAxis"), 1, glm::value_ptr(tubeLight.GetTubeAxis()));
    glUniform1f(glGetUniformLocation(shaderID, "tubeLength"), tubeLight.GetTubeLength());
    glUniform1f(glGetUniformLocation(shaderID, "tubeRadius"), tubeLight.GetTubeRadius());
    glUniform3fv(glGetUniformLocation(shaderID, "lightColor"), 1, glm::value_ptr(color));
    if (viewPos)
        glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, glm::value_ptr(*viewPos));
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(window::width, window::height, window::title, NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    for (int i = 0; i < furniture::fans; i++)
        fanRotationSpeed[i] = 2.0f;

    float roomLength = room::length, roomWidth = room::width, roomHeight = room::height;

    // Window parameters
    const float frameThickness = 0.05f;
    const float windowHeight = roomHeight * 0.20f;
    const float topMargin = 0.30f;
    const float windowTopY = roomHeight - topMargin;
    const float windowBottomY = windowTopY - windowHeight;
    const float sideMargin = frameThickness + 0.05f;
    const float windowSpacing = 0.50f;
    const int numBackWindows = 8;
    const float windowWidth = (roomLength - 2.0f * sideMargin - windowSpacing * (numBackWindows - 1)) / numBackWindows;
    const float startX = -roomLength / 2.0f + sideMargin;

    // Door parameters
    const float doorHeight = 4.2f, doorWidth = 2.0f;
    const float doorTopY = doorHeight;
    const float frontWallZ = roomWidth / 2.0f;
    const float doorCenterZ = frontWallZ - 2.5f;
    const float doorBackZ = doorCenterZ - doorWidth / 2.0f;
    const float doorFrontZ = doorCenterZ + doorWidth / 2.0f;

    // Create walls
    std::vector<GLfloat> backWallVertices, rightWallVertices;
    std::vector<GLuint> backWallIndices, rightWallIndices;

    createBackWall(backWallVertices, backWallIndices, roomLength, roomWidth, roomHeight,
                   windowBottomY, windowTopY, windowWidth, startX, windowSpacing, numBackWindows);

    createRightWall(rightWallVertices, rightWallIndices, roomLength, roomWidth, roomHeight,
                    windowBottomY, windowTopY, windowWidth, 0.5f, 0.5f, windowSpacing,
                    doorBackZ, doorFrontZ, doorTopY);

    GLfloat vertices[] = {
        -roomLength / 2, 0.0f, -roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,
        roomLength / 2, 0.0f, -roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,
        roomLength / 2, 0.0f, roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,
        -roomLength / 2, 0.0f, roomWidth / 2, 0.98f, 0.98f, 0.98f, 0.0f, 1.0f, 0.0f,

        -roomLength / 2, 0.0f, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,
        roomLength / 2, 0.0f, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,
        roomLength / 2, roomHeight, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,
        -roomLength / 2, roomHeight, roomWidth / 2, 0.95f, 0.90f, 0.70f, 0.0f, 0.0f, -1.0f,

        -roomLength / 2, 0.0f, -roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, 0.0f, roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, roomHeight, roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, roomHeight, -roomWidth / 2, 0.95f, 0.90f, 0.70f, 1.0f, 0.0f, 0.0f};

    GLuint indices[] = {0, 1, 2, 2, 3, 0, 4, 6, 5, 6, 4, 7, 8, 10, 9, 10, 8, 11};

    Shader roomShader("shaders/default.vert", "shaders/default.frag");
    Shader furnitureShader("shaders/texture.vert", "shaders/texture.frag");

    VAO roomVAO;
    roomVAO.Bind();
    VBO roomVBO(vertices, sizeof(vertices));
    EBO roomEBO(indices, sizeof(indices));
    roomVAO.LinkVBOAttrib(roomVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    roomVAO.LinkVBOAttrib(roomVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    roomVAO.LinkVBOAttrib(roomVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    roomVAO.Unbind();

    VAO backWallVAO;
    backWallVAO.Bind();
    VBO backWallVBO(backWallVertices.data(), backWallVertices.size() * sizeof(GLfloat));
    EBO backWallEBO(backWallIndices.data(), backWallIndices.size() * sizeof(GLuint));
    backWallVAO.LinkVBOAttrib(backWallVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    backWallVAO.LinkVBOAttrib(backWallVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    backWallVAO.LinkVBOAttrib(backWallVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    backWallVAO.Unbind();

    VAO rightWallVAO;
    rightWallVAO.Bind();
    VBO rightWallVBO(rightWallVertices.data(), rightWallVertices.size() * sizeof(GLfloat));
    EBO rightWallEBO(rightWallIndices.data(), rightWallIndices.size() * sizeof(GLuint));
    rightWallVAO.LinkVBOAttrib(rightWallVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    rightWallVAO.LinkVBOAttrib(rightWallVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    rightWallVAO.LinkVBOAttrib(rightWallVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    rightWallVAO.Unbind();

    // Load models
    Model customDesk("models/desk.obj");
    Model customFan("models/classroom_fan.obj");
    Model customPodium("models/podium.obj");
    Model customProjector("models/classroom_projector.obj");
    Model projectorScreenRod("models/project_screen_rod.obj");

    CeilingTiles ceilingTiles(roomLength, roomWidth, roomHeight, 10, 15);
    LightPanels lightPanels(roomLength, roomWidth, roomHeight, 10, 15, 8, 3, 8, 7);
    TubeLight tubeLight(roomLength, roomWidth, roomHeight);
    Windows backWallWindows(roomLength, roomWidth, roomHeight, 8);
    RightWallWindows rightWallWindows(roomLength, roomWidth, roomHeight);
    GreenBoard greenBoards(roomLength, roomWidth, roomHeight);
    Door entranceDoor(roomLength, roomWidth, roomHeight);
    projectorScreen = new ProjectorScreen(roomLength, roomWidth, roomHeight);

    // Calculate light positions
    const float tileWidth = roomLength / 15;
    const float tileHeight = roomWidth / 10;
    const float lightY = roomHeight - 0.3f;
    lightPos1 = glm::vec3(-roomLength / 2.0f + 3.5f * tileWidth, lightY, -roomWidth / 2.0f + 8.5f * tileHeight);
    lightPos2 = glm::vec3(-roomLength / 2.0f + 7.5f * tileWidth, lightY, -roomWidth / 2.0f + 8.5f * tileHeight);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (projectorScreen)
            projectorScreen->Update(deltaTime);

        processInput(window);

        glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f),
                                                (float)window::width / window::height, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        // Render room
        roomShader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(roomShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(roomShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(roomShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        setLightingUniforms(roomShader.ID, lightPos1, lightPos2, tubeLight, lightColor);

        roomVAO.Bind();
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        backWallVAO.Bind();
        glDrawElements(GL_TRIANGLES, backWallIndices.size(), GL_UNSIGNED_INT, 0);
        rightWallVAO.Bind();
        glDrawElements(GL_TRIANGLES, rightWallIndices.size(), GL_UNSIGNED_INT, 0);

        ceilingTiles.Draw(roomShader, model, view, projection);
        lightPanels.Draw(model, view, projection);
        tubeLight.Draw(model, view, projection);
        backWallWindows.Draw(roomShader, model, view, projection);
        rightWallWindows.Draw(roomShader, model, view, projection);
        greenBoards.Draw(roomShader, model, view, projection);
        if (projectorScreen)
            projectorScreen->Draw(roomShader, model, view, projection);
        entranceDoor.Draw(roomShader, model, view, projection);

        // Render furniture
        furnitureShader.Activate();
        setLightingUniforms(furnitureShader.ID, lightPos1, lightPos2, tubeLight, lightColor, &cameraPos);
        glUniform1i(glGetUniformLocation(furnitureShader.ID, "isWhitePlastic"), 0);

        // Render desks
        const float deskScale = furniture::deskScale;
        const float deskYPos = 1.6f;
        const float colSpacing = 5.0f, rowSpacing = 3.5f;
        const float benchWidth = 1.8f;
        const float backMargin = 2.5f;
        const float startZ = -roomWidth / 2 + backMargin;
        const float gridWidth = (furniture::cols - 1) * colSpacing + benchWidth;
        const float sideSpace = (roomLength - gridWidth) / 2.0f;
        const float startXDesk = -roomLength / 2 + sideSpace + benchWidth / 2.0f;

        for (int row = 0; row < furniture::rows; row++)
        {
            for (int col = 0; col < furniture::cols; col++)
            {
                glm::mat4 deskModel = glm::mat4(1.0f);
                deskModel = glm::translate(deskModel, glm::vec3(startXDesk + col * colSpacing,
                                                                deskYPos, startZ + row * rowSpacing));
                deskModel = glm::scale(deskModel, glm::vec3(deskScale));
                deskModel = glm::rotate(deskModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                customDesk.Draw(furnitureShader, deskModel, view, projection);
            }
        }

        // Render fans
        const float fanScale = furniture::fanScale;
        const float fanYPos = roomHeight - 1.2f;
        const float fanSpacingX = roomLength * 0.7f / (furniture::fanCols - 1);
        const float fanSpacingZ = roomWidth * 0.6f / (furniture::fanRows - 1);
        const float fanStartX = -roomLength * 0.35f;
        const float fanStartZ = -roomWidth * 0.3f;

        int fanIndex = 0;
        for (int row = 0; row < furniture::fanRows; row++)
        {
            for (int col = 0; col < furniture::fanCols; col++)
            {
                float rotation = fmod(glfwGetTime() * fanRotationSpeed[fanIndex] * 360.0f + fanIndex * 45.0f, 360.0f);
                glm::mat4 fanModel = glm::mat4(1.0f);
                fanModel = glm::translate(fanModel, glm::vec3(fanStartX + col * fanSpacingX,
                                                              fanYPos, fanStartZ + row * fanSpacingZ));
                fanModel = glm::scale(fanModel, glm::vec3(fanScale));
                fanModel = glm::rotate(fanModel, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
                customFan.Draw(furnitureShader, fanModel, view, projection);
                fanIndex++;
            }
        }

        // Render podium
        glm::mat4 podiumModel = glm::mat4(1.0f);
        podiumModel = glm::translate(podiumModel, glm::vec3(roomLength / 2 - 5.5f, 1.35f, roomWidth / 2 - 2.0f));
        podiumModel = glm::scale(podiumModel, glm::vec3(1.2f));
        podiumModel = glm::rotate(podiumModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        customPodium.Draw(furnitureShader, podiumModel, view, projection);

        // Render projector
        glUniform1i(glGetUniformLocation(furnitureShader.ID, "isWhitePlastic"), 1);
        glm::mat4 projectorModel = glm::mat4(1.0f);
        projectorModel = glm::translate(projectorModel, glm::vec3(0.0f, roomHeight - 2.2f, 0.0f));
        projectorModel = glm::scale(projectorModel, glm::vec3(0.3f));
        projectorModel = glm::rotate(projectorModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        customProjector.Draw(furnitureShader, projectorModel, view, projection);

        // Render screen rod
        glUniform1i(glGetUniformLocation(furnitureShader.ID, "isWhitePlastic"), 0);
        float boardHeight = roomHeight * 0.35f;
        float boardTopY = roomHeight / 2.0f + boardHeight / 2.0f;
        glm::mat4 screenRodModel = glm::mat4(1.0f);
        screenRodModel = glm::translate(screenRodModel, glm::vec3(0.0f, boardTopY + 0.3f, frontWallZ - 0.15f));
        screenRodModel = glm::scale(screenRodModel, glm::vec3(0.5f));
        screenRodModel = glm::rotate(screenRodModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projectorScreenRod.Draw(furnitureShader, screenRodModel, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
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

    customDesk.Delete();
    customFan.Delete();
    customPodium.Delete();
    customProjector.Delete();
    projectorScreenRod.Delete();
    ceilingTiles.Delete();
    lightPanels.Delete();
    tubeLight.Delete();
    backWallWindows.Delete();
    rightWallWindows.Delete();
    greenBoards.Delete();
    entranceDoor.Delete();

    if (projectorScreen)
    {
        projectorScreen->Delete();
        delete projectorScreen;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}