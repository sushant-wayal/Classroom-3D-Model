#include "../../include/Door.h"
#include <iostream>

Door::Door(float roomLength, float roomWidth, float roomHeight)
{
    // Initialize pointers
    doorVBO = nullptr;
    doorEBO = nullptr;
    frameVBO = nullptr;
    frameEBO = nullptr;

    // Standard door dimensions - INCREASED BY 2x (slightly smaller than before)
    this->doorHeight = 2.1f * 2.0f;      // 4.2m height (2x bigger)
    this->doorWidth = 1.0f * 2.0f;       // 2.0m width (2x bigger)
    this->frameThickness = 0.08f * 2.0f; // 16cm thick frame (2x bigger)

    float frameDepth = 0.25f; // Frame extends 25cm from wall inward

    // Right wall position
    float rightWallX = roomLength / 2.0f;

    // Door positioned on the floor
    float doorBottomY = 0.0f;
    float doorTopY = doorBottomY + doorHeight;

    // Position door SIGNIFICANTLY TOWARDS THE FRONT - very close to front wall
    // Place it RIGHT BEFORE the front window, much closer to front wall
    float frontMargin = 0.5f;
    float frontWallZ = roomWidth / 2.0f;

    // Position door very close to front wall - only 2.5m from front wall!
    float doorCenterZ = frontWallZ - frontMargin - 2.0f; // Only 2.5m from front wall (was ~4.5m before)

    float doorBackZ = doorCenterZ - doorWidth / 2.0f;
    float doorFrontZ = doorCenterZ + doorWidth / 2.0f;

    std::vector<GLfloat> doorVertices;
    std::vector<GLuint> doorIndices;
    std::vector<GLfloat> frameVertices;
    std::vector<GLuint> frameIndices;

    // === DOOR PANEL - wooden door ===
    float doorX = rightWallX - frameDepth * 0.7f; // Door slightly inward
    unsigned int doorBaseIndex = 0;

    // Brown wooden door color
    float doorR = 0.45f, doorG = 0.27f, doorB = 0.15f;

    // Door panel facing inward (toward -X direction)
    doorVertices.insert(doorVertices.end(), {doorX, doorBottomY, doorBackZ, doorR, doorG, doorB, -1.0f, 0.0f, 0.0f,
                                             doorX, doorBottomY, doorFrontZ, doorR, doorG, doorB, -1.0f, 0.0f, 0.0f,
                                             doorX, doorTopY, doorFrontZ, doorR, doorG, doorB, -1.0f, 0.0f, 0.0f,
                                             doorX, doorTopY, doorBackZ, doorR, doorG, doorB, -1.0f, 0.0f, 0.0f});

    doorIndices.insert(doorIndices.end(), {doorBaseIndex + 0, doorBaseIndex + 1, doorBaseIndex + 2,
                                           doorBaseIndex + 2, doorBaseIndex + 3, doorBaseIndex + 0});

    // Add door panel thickness (sides and back)
    float doorThickness = 0.05f;
    float doorBackX = doorX - doorThickness;
    unsigned int backIdx = doorVertices.size() / 9;

    // Back face of door
    doorVertices.insert(doorVertices.end(), {doorBackX, doorBottomY, doorBackZ, doorR, doorG, doorB, 1.0f, 0.0f, 0.0f,
                                             doorBackX, doorBottomY, doorFrontZ, doorR, doorG, doorB, 1.0f, 0.0f, 0.0f,
                                             doorBackX, doorTopY, doorFrontZ, doorR, doorG, doorB, 1.0f, 0.0f, 0.0f,
                                             doorBackX, doorTopY, doorBackZ, doorR, doorG, doorB, 1.0f, 0.0f, 0.0f});

    doorIndices.insert(doorIndices.end(), {backIdx + 0, backIdx + 2, backIdx + 1,
                                           backIdx + 2, backIdx + 0, backIdx + 3});

    // Door edges (top, bottom, left, right)
    unsigned int edgeIdx = doorVertices.size() / 9;

    // Bottom edge
    doorVertices.insert(doorVertices.end(), {doorX, doorBottomY, doorBackZ, doorR, doorG, doorB, 0.0f, -1.0f, 0.0f,
                                             doorX, doorBottomY, doorFrontZ, doorR, doorG, doorB, 0.0f, -1.0f, 0.0f,
                                             doorBackX, doorBottomY, doorFrontZ, doorR, doorG, doorB, 0.0f, -1.0f, 0.0f,
                                             doorBackX, doorBottomY, doorBackZ, doorR, doorG, doorB, 0.0f, -1.0f, 0.0f});
    doorIndices.insert(doorIndices.end(), {edgeIdx + 0, edgeIdx + 1, edgeIdx + 2,
                                           edgeIdx + 2, edgeIdx + 3, edgeIdx + 0});

    edgeIdx = doorVertices.size() / 9;
    // Top edge
    doorVertices.insert(doorVertices.end(), {doorX, doorTopY, doorBackZ, doorR, doorG, doorB, 0.0f, 1.0f, 0.0f,
                                             doorX, doorTopY, doorFrontZ, doorR, doorG, doorB, 0.0f, 1.0f, 0.0f,
                                             doorBackX, doorTopY, doorFrontZ, doorR, doorG, doorB, 0.0f, 1.0f, 0.0f,
                                             doorBackX, doorTopY, doorBackZ, doorR, doorG, doorB, 0.0f, 1.0f, 0.0f});
    doorIndices.insert(doorIndices.end(), {edgeIdx + 0, edgeIdx + 2, edgeIdx + 1,
                                           edgeIdx + 2, edgeIdx + 0, edgeIdx + 3});

    edgeIdx = doorVertices.size() / 9;
    // Left edge
    doorVertices.insert(doorVertices.end(), {doorX, doorBottomY, doorBackZ, doorR, doorG, doorB, 0.0f, 0.0f, -1.0f,
                                             doorBackX, doorBottomY, doorBackZ, doorR, doorG, doorB, 0.0f, 0.0f, -1.0f,
                                             doorBackX, doorTopY, doorBackZ, doorR, doorG, doorB, 0.0f, 0.0f, -1.0f,
                                             doorX, doorTopY, doorBackZ, doorR, doorG, doorB, 0.0f, 0.0f, -1.0f});
    doorIndices.insert(doorIndices.end(), {edgeIdx + 0, edgeIdx + 1, edgeIdx + 2,
                                           edgeIdx + 2, edgeIdx + 3, edgeIdx + 0});

    edgeIdx = doorVertices.size() / 9;
    // Right edge
    doorVertices.insert(doorVertices.end(), {doorX, doorBottomY, doorFrontZ, doorR, doorG, doorB, 0.0f, 0.0f, 1.0f,
                                             doorBackX, doorBottomY, doorFrontZ, doorR, doorG, doorB, 0.0f, 0.0f, 1.0f,
                                             doorBackX, doorTopY, doorFrontZ, doorR, doorG, doorB, 0.0f, 0.0f, 1.0f,
                                             doorX, doorTopY, doorFrontZ, doorR, doorG, doorB, 0.0f, 0.0f, 1.0f});
    doorIndices.insert(doorIndices.end(), {edgeIdx + 0, edgeIdx + 2, edgeIdx + 1,
                                           edgeIdx + 2, edgeIdx + 0, edgeIdx + 3});

    // === DOOR FRAME - white frame around door ===
    float frameOuterBackZ = doorBackZ - frameThickness;
    float frameOuterFrontZ = doorFrontZ + frameThickness;
    float frameOuterBottomY = doorBottomY;
    float frameOuterTopY = doorTopY + frameThickness;

    float frameInnerBackZ = doorBackZ;
    float frameInnerFrontZ = doorFrontZ;
    float frameInnerBottomY = doorBottomY;
    float frameInnerTopY = doorTopY;

    float frameR = 1.0f, frameG = 1.0f, frameB = 1.0f;
    unsigned int frameBaseIndex = 0;
    float innerX = rightWallX - frameDepth;

    // Front face of frame (at wall surface)
    frameVertices.insert(frameVertices.end(), {// Outer rectangle
                                               rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                               rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                               rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                               rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                               // Inner rectangle (door opening)
                                               rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                               rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                               rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                               rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f});

    // Front face frame (4 strips around door opening)
    frameIndices.insert(frameIndices.end(), {// Bottom strip
                                             frameBaseIndex + 0, frameBaseIndex + 1, frameBaseIndex + 5,
                                             frameBaseIndex + 5, frameBaseIndex + 4, frameBaseIndex + 0,
                                             // Right strip
                                             frameBaseIndex + 1, frameBaseIndex + 2, frameBaseIndex + 6,
                                             frameBaseIndex + 6, frameBaseIndex + 5, frameBaseIndex + 1,
                                             // Top strip
                                             frameBaseIndex + 2, frameBaseIndex + 3, frameBaseIndex + 7,
                                             frameBaseIndex + 7, frameBaseIndex + 6, frameBaseIndex + 2,
                                             // Left strip
                                             frameBaseIndex + 3, frameBaseIndex + 0, frameBaseIndex + 4,
                                             frameBaseIndex + 4, frameBaseIndex + 7, frameBaseIndex + 3});

    // Inner frame edges (around door opening)
    unsigned int innerFrameIdx = frameVertices.size() / 9;

    // Bottom inner
    frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                               rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                               innerX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                               innerX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f});
    frameIndices.insert(frameIndices.end(), {innerFrameIdx + 0, innerFrameIdx + 2, innerFrameIdx + 1,
                                             innerFrameIdx + 2, innerFrameIdx + 0, innerFrameIdx + 3});

    innerFrameIdx = frameVertices.size() / 9;
    // Top inner
    frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                               rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                               innerX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                               innerX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f});
    frameIndices.insert(frameIndices.end(), {innerFrameIdx + 0, innerFrameIdx + 1, innerFrameIdx + 2,
                                             innerFrameIdx + 2, innerFrameIdx + 3, innerFrameIdx + 0});

    innerFrameIdx = frameVertices.size() / 9;
    // Left inner
    frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                               innerX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                               innerX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                               rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f});
    frameIndices.insert(frameIndices.end(), {innerFrameIdx + 0, innerFrameIdx + 1, innerFrameIdx + 2,
                                             innerFrameIdx + 2, innerFrameIdx + 3, innerFrameIdx + 0});

    innerFrameIdx = frameVertices.size() / 9;
    // Right inner
    frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                               innerX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                               innerX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                               rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f});
    frameIndices.insert(frameIndices.end(), {innerFrameIdx + 0, innerFrameIdx + 2, innerFrameIdx + 1,
                                             innerFrameIdx + 2, innerFrameIdx + 0, innerFrameIdx + 3});

    // Outer frame edges
    unsigned int outerIdx = frameVertices.size() / 9;

    // Bottom outer
    frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                               rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                               innerX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                               innerX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f});
    frameIndices.insert(frameIndices.end(), {outerIdx + 0, outerIdx + 1, outerIdx + 2,
                                             outerIdx + 2, outerIdx + 3, outerIdx + 0});

    outerIdx = frameVertices.size() / 9;
    // Top outer
    frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                               rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                               innerX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                               innerX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f});
    frameIndices.insert(frameIndices.end(), {outerIdx + 0, outerIdx + 2, outerIdx + 1,
                                             outerIdx + 2, outerIdx + 0, outerIdx + 3});

    outerIdx = frameVertices.size() / 9;
    // Left outer
    frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                               innerX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                               innerX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                               rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f});
    frameIndices.insert(frameIndices.end(), {outerIdx + 0, outerIdx + 2, outerIdx + 1,
                                             outerIdx + 2, outerIdx + 0, outerIdx + 3});

    outerIdx = frameVertices.size() / 9;
    // Right outer
    frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                               innerX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                               innerX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                               rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f});
    frameIndices.insert(frameIndices.end(), {outerIdx + 0, outerIdx + 1, outerIdx + 2,
                                             outerIdx + 2, outerIdx + 3, outerIdx + 0});

    numDoorIndices = doorIndices.size();
    numFrameIndices = frameIndices.size();

    // Create VAO/VBO/EBO for door
    doorVAO.Bind();
    doorVBO = new VBO(doorVertices.data(), doorVertices.size() * sizeof(GLfloat));
    doorEBO = new EBO(doorIndices.data(), doorIndices.size() * sizeof(GLuint));
    doorVAO.LinkVBOAttrib(*doorVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    doorVAO.LinkVBOAttrib(*doorVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    doorVAO.LinkVBOAttrib(*doorVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    doorVAO.Unbind();
    doorVBO->Unbind();
    doorEBO->Unbind();

    // Create VAO/VBO/EBO for frame
    frameVAO.Bind();
    frameVBO = new VBO(frameVertices.data(), frameVertices.size() * sizeof(GLfloat));
    frameEBO = new EBO(frameIndices.data(), frameIndices.size() * sizeof(GLuint));
    frameVAO.LinkVBOAttrib(*frameVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    frameVAO.LinkVBOAttrib(*frameVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    frameVAO.LinkVBOAttrib(*frameVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    frameVAO.Unbind();
    frameVBO->Unbind();
    frameEBO->Unbind();
}

void Door::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw white frame first
    glUniform1f(glGetUniformLocation(shader.ID, "transparency"), 1.0f);
    frameVAO.Bind();
    glDrawElements(GL_TRIANGLES, numFrameIndices, GL_UNSIGNED_INT, 0);

    // Draw wooden door panel
    glUniform1f(glGetUniformLocation(shader.ID, "transparency"), 1.0f);
    doorVAO.Bind();
    glDrawElements(GL_TRIANGLES, numDoorIndices, GL_UNSIGNED_INT, 0);
}

void Door::Delete()
{
    doorVAO.Delete();
    if (doorVBO)
    {
        doorVBO->Delete();
        delete doorVBO;
        doorVBO = nullptr;
    }
    if (doorEBO)
    {
        doorEBO->Delete();
        delete doorEBO;
        doorEBO = nullptr;
    }

    frameVAO.Delete();
    if (frameVBO)
    {
        frameVBO->Delete();
        delete frameVBO;
        frameVBO = nullptr;
    }
    if (frameEBO)
    {
        frameEBO->Delete();
        delete frameEBO;
        frameEBO = nullptr;
    }
}
