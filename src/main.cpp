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
#include "models/Model.h" // Include the Model class

// Camera variables - Adjusted for better classroom overview
glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 15.0f);    // Higher and further back
glm::vec3 cameraFront = glm::vec3(0.0f, -0.2f, -1.0f); // Slight downward angle
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = -10.0f; // Slight downward angle
float lastX = window::width / 2.0f;
float lastY = window::height / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Lighting - Better positioned for classroom visibility
glm::vec3 lightPos = glm::vec3(0.0f, 6.0f, 0.0f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f); // Slightly warm light

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
    glfwWindowHint(GLFW_SAMPLES, 4); // Reduced from 8 to 4 for better compatibility

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

    // Configure global opengl state - back to simple settings
    glEnable(GL_DEPTH_TEST);

    // Enable multisampling for smoother edges
    glEnable(GL_MULTISAMPLE);

    // Classroom dimensions (in meters, scaled for OpenGL)
    float roomLength = room::length;
    float roomWidth = room::width;
    float roomHeight = room::height;

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

    // Create shader program for furniture with lighting
    Shader furnitureShader("shaders/texture.vert", "shaders/texture.frag");

    // Create vertex array and buffers for room
    VAO roomVAO;
    roomVAO.Bind();

    VBO roomVBO(vertices, sizeof(vertices));
    EBO roomEBO(indices, sizeof(indices));

    // Position attribute (location = 0)
    roomVAO.LinkVBOAttrib(roomVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    // Color attribute (location = 1)
    roomVAO.LinkVBOAttrib(roomVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    // Normal attribute (location = 2)
    roomVAO.LinkVBOAttrib(roomVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));

    roomVAO.Unbind();
    roomVBO.Unbind();
    roomEBO.Unbind();

    // Create furniture pieces - Simple texture application
    std::cout << "Loading custom Blender models..." << std::endl;

    // Load custom desk model with simple texture application
    Model customDesk("models/classroom_desk.obj", "textures/desk_texture.jpeg");
    std::cout << "Custom desk model loaded with wood texture applied to entire model!" << std::endl;

    Model customFan("models/classroom_fan.obj");
    std::cout << "Custom Fan Model loaded successfully!" << std::endl;

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

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create transformations
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), // Increased FOV from 45 to 60
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

        // Render furniture with lighting
        furnitureShader.Activate();

        // Set lighting uniforms
        glUniform3fv(glGetUniformLocation(furnitureShader.ID, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(furnitureShader.ID, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(furnitureShader.ID, "viewPos"), 1, glm::value_ptr(cameraPos));

        // Render desks using custom Blender model - 3 rows of 4 desks
        // Scale factor to make desk proportional to classroom size
        float deskScale = furniture::deskScale; // Increased from 0.3 to 0.5 for better proportion

        int noOfRows = furniture::rows;
        int noOfCols = furniture::cols;

        float deskYPos = 0.0f;

        for (int row = 0; row < noOfRows; row++)
        {
            for (int col = 0; col < noOfCols; col++)
            {
                glm::mat4 deskModel = glm::mat4(1.0f);

                // Position desks with all rows moved one position BACKWARD (more negative z)
                float x = -7.5f + col * 5.0f; // Wider spacing for larger desks
                float z = -5.0f + row * 4.0f; // Moved all rows BACK: start at z=-5.0 (was z=-1.0)
                                              // Row 1: z=-5.0, Row 2: z=-1.0, Row 3: z=3.0

                // Apply transformations in correct order
                deskModel = glm::translate(deskModel, glm::vec3(x, deskYPos, z));
                deskModel = glm::scale(deskModel, glm::vec3(deskScale, deskScale, deskScale));
                // Rotate 180 degrees around Y-axis to face the blackboard (front of classroom)
                deskModel = glm::rotate(deskModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

                customDesk.Draw(furnitureShader, deskModel, view, projection);
            }
        }

        float fanScale = furniture::fanScale;

        int noOfFans = furniture::fans;

        float fanYPos = roomHeight - 1.2f;
        float fanZPos = 0.0f;

        float distanceBetween = 9.0f;

        // Add fan rotation for more realistic appearance
        float fanRotationSpeed = 2.0f; // Set to 0 to stop rotation and focus on smoothness
        float fanRotation = fmod(currentFrame * fanRotationSpeed * 360.0f, 360.0f);

        for (int fan = 0; fan < noOfFans; fan++)
        {
            glm::mat4 fanModel = glm::mat4(1.0f);

            float x = ((roomLength - distanceBetween) / 2) - (roomLength / 2) + fan * distanceBetween;

            fanModel = glm::translate(fanModel, glm::vec3(x, fanYPos, fanZPos));
            fanModel = glm::scale(fanModel, glm::vec3(fanScale, fanScale, fanScale));
            // Add rotation around Y-axis (vertical) for fan blades
            fanModel = glm::rotate(fanModel, glm::radians(fanRotation), glm::vec3(0.0f, 1.0f, 0.0f));

            customFan.Draw(furnitureShader, fanModel, view, projection);
        }

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    roomVAO.Delete();
    roomVBO.Delete();
    roomEBO.Delete();
    roomShader.Delete();
    furnitureShader.Delete();

    // Clean up furniture
    customDesk.Delete();
    customFan.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}