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

glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.2f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = -10.0f;
float lastX = window::width / 2.0f;
float lastY = window::height / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting
glm::vec3 lightPos = glm::vec3(0.0f, 6.0f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);

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
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        // Left side view
        cameraPos = glm::vec3(-10.0f, 3.0f, 2.0f);
        cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
        yaw = 0.0f;
        pitch = 0.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
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

    // Classroom dimensions
    float roomLength = 24.0f;
    float roomWidth = 16.0f;
    float roomHeight = 7.0f;

    // Vertices for classroom walls, floor, and ceiling with colors and normals
    // Format: x, y, z, r, g, b, nx, ny, nz
    GLfloat vertices[] = {
        // Floor (y = 0) - Brown color
        -roomLength / 2, 0.0f, -roomWidth / 2, 0.6f, 0.4f, 0.2f, 0.0f, 1.0f, 0.0f,
        roomLength / 2, 0.0f, -roomWidth / 2, 0.6f, 0.4f, 0.2f, 0.0f, 1.0f, 0.0f,
        roomLength / 2, 0.0f, roomWidth / 2, 0.6f, 0.4f, 0.2f, 0.0f, 1.0f, 0.0f,
        -roomLength / 2, 0.0f, roomWidth / 2, 0.6f, 0.4f, 0.2f, 0.0f, 1.0f, 0.0f,

        // Ceiling (y = roomHeight) - Light gray
        -roomLength / 2, roomHeight, -roomWidth / 2, 0.9f, 0.9f, 0.9f, 0.0f, -1.0f, 0.0f,
        roomLength / 2, roomHeight, -roomWidth / 2, 0.9f, 0.9f, 0.9f, 0.0f, -1.0f, 0.0f,
        roomLength / 2, roomHeight, roomWidth / 2, 0.9f, 0.9f, 0.9f, 0.0f, -1.0f, 0.0f,
        -roomLength / 2, roomHeight, roomWidth / 2, 0.9f, 0.9f, 0.9f, 0.0f, -1.0f, 0.0f,

        // Front wall (z = roomWidth/2) - Light blue
        -roomLength / 2, 0.0f, roomWidth / 2, 0.7f, 0.8f, 1.0f, 0.0f, 0.0f, -1.0f,
        roomLength / 2, 0.0f, roomWidth / 2, 0.7f, 0.8f, 1.0f, 0.0f, 0.0f, -1.0f,
        roomLength / 2, roomHeight, roomWidth / 2, 0.7f, 0.8f, 1.0f, 0.0f, 0.0f, -1.0f,
        -roomLength / 2, roomHeight, roomWidth / 2, 0.7f, 0.8f, 1.0f, 0.0f, 0.0f, -1.0f,

        // Back wall (z = -roomWidth/2) - Light green
        -roomLength / 2, 0.0f, -roomWidth / 2, 0.7f, 1.0f, 0.7f, 0.0f, 0.0f, 1.0f,
        roomLength / 2, 0.0f, -roomWidth / 2, 0.7f, 1.0f, 0.7f, 0.0f, 0.0f, 1.0f,
        roomLength / 2, roomHeight, -roomWidth / 2, 0.7f, 1.0f, 0.7f, 0.0f, 0.0f, 1.0f,
        -roomLength / 2, roomHeight, -roomWidth / 2, 0.7f, 1.0f, 0.7f, 0.0f, 0.0f, 1.0f,

        // Left wall (x = -roomLength/2) - Light yellow
        -roomLength / 2, 0.0f, -roomWidth / 2, 1.0f, 1.0f, 0.7f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, 0.0f, roomWidth / 2, 1.0f, 1.0f, 0.7f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, roomHeight, roomWidth / 2, 1.0f, 1.0f, 0.7f, 1.0f, 0.0f, 0.0f,
        -roomLength / 2, roomHeight, -roomWidth / 2, 1.0f, 1.0f, 0.7f, 1.0f, 0.0f, 0.0f,

        // Right wall (x = roomLength/2) - Light pink
        roomLength / 2, 0.0f, -roomWidth / 2, 1.0f, 0.8f, 0.9f, -1.0f, 0.0f, 0.0f,
        roomLength / 2, 0.0f, roomWidth / 2, 1.0f, 0.8f, 0.9f, -1.0f, 0.0f, 0.0f,
        roomLength / 2, roomHeight, roomWidth / 2, 1.0f, 0.8f, 0.9f, -1.0f, 0.0f, 0.0f,
        roomLength / 2, roomHeight, -roomWidth / 2, 1.0f, 0.8f, 0.9f, -1.0f, 0.0f, 0.0f};

    GLuint indices[] = {
        // Floor
        0, 1, 2, 2, 3, 0,
        // Ceiling
        4, 6, 5, 6, 4, 7,
        // Front wall
        8, 9, 10, 10, 11, 8,
        // Back wall
        12, 14, 13, 14, 12, 15,
        // Left wall
        16, 17, 18, 18, 19, 16,
        // Right wall
        20, 22, 21, 22, 20, 23};

    // Create shader program for basic room
    Shader roomShader("shaders/default.vert", "shaders/default.frag");

    // Create vertex array and buffers for room
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

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), // Increased FOV from 45 to 60
                                                (float)window::width / (float)window::height,
                                                0.1f, 100.0f);

        // Render room
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

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    roomVAO.Delete();
    roomVBO.Delete();
    roomEBO.Delete();
    roomShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}