#include "../../include/RightWallWindows.h"
#include <iostream>

RightWallWindows::RightWallWindows(float roomLength, float roomWidth, float roomHeight)
{
    // Initialize pointers
    glassVBO = nullptr;
    glassEBO = nullptr;
    frameVBO = nullptr;
    frameEBO = nullptr;

    // Use same dimensions as back wall windows
    this->windowHeight = roomHeight * 0.20f; // 20% of room height (1.4m for 7m ceiling)
    this->windowWidth = 2.7875f;             // Same width as back wall windows
    this->frameThickness = 0.05f;            // 5cm thick frames

    float frameDepth = 0.25f; // Frame extends 25cm from wall inward

    std::cout << "\n=== Creating RIGHT WALL WINDOWS (BEVELED frames) ===" << std::endl;
    std::cout << "Total windows: 6 (5 back section + 1 front section)" << std::endl;
    std::cout << "Window dimensions: " << windowWidth << "m x " << windowHeight << "m (RECTANGULAR)" << std::endl;
    std::cout << "Frame thickness: " << frameThickness << "m (BEVELED/SLOPED inward)" << std::endl;

    // Right wall position
    float rightWallX = roomLength / 2.0f;

    // Windows positioned at TOP with same margin as back wall
    float topMargin = 0.30f; // 30cm from ceiling
    float windowTopY = roomHeight - topMargin;
    float windowBottomY = windowTopY - windowHeight;

    std::cout << "Window Y position: " << windowBottomY << "m to " << windowTopY << "m" << std::endl;

    // Z-axis positioning along the right wall
    float backMargin = 0.5f;            // Start 0.5m from back wall
    float frontMargin = 0.5f;           // End 0.5m from front wall
    float spacingBetweenWindows = 0.5f; // 50cm gap between adjacent windows

    // BACK SECTION: 5 windows
    int numBackWindows = 5;
    float backSectionStart = -roomWidth / 2.0f + backMargin;
    float backSectionLength = numBackWindows * windowWidth + (numBackWindows - 1) * spacingBetweenWindows;

    // FRONT SECTION: 1 window (CHANGED from 2 to 1)
    int numFrontWindows = 1;
    float frontSectionEnd = roomWidth / 2.0f - frontMargin;
    float frontSectionLength = numFrontWindows * windowWidth; // No spacing needed for single window
    float frontSectionStart = frontSectionEnd - frontSectionLength;

    // Calculate the gap between sections
    float gapBetweenSections = frontSectionStart - (backSectionStart + backSectionLength);

    std::cout << "\nBACK SECTION (5 windows):" << std::endl;
    std::cout << "  Start Z: " << backSectionStart << "m" << std::endl;
    std::cout << "  End Z: " << (backSectionStart + backSectionLength) << "m" << std::endl;
    std::cout << "\nFRONT SECTION (1 window):" << std::endl;
    std::cout << "  Start Z: " << frontSectionStart << "m" << std::endl;
    std::cout << "  End Z: " << frontSectionEnd << "m" << std::endl;
    std::cout << "\nGAP between sections: " << gapBetweenSections << "m (SIGNIFICANT!)" << std::endl;

    std::vector<GLfloat> glassVertices;
    std::vector<GLuint> glassIndices;
    std::vector<GLfloat> frameVertices;
    std::vector<GLuint> frameIndices;

    std::cout << "\nIndividual window positions:" << std::endl;

    // Lambda function to create a window at given Z position
    auto createWindow = [&](int windowNum, float windowBackZ, float windowFrontZ)
    {
        std::cout << "  Window " << windowNum << ": Z[" << windowBackZ << " to " << windowFrontZ
                  << "] = " << windowWidth << "m deep" << std::endl;

        // === GLASS - centered in opening ===
        float glassX = rightWallX - frameDepth * 0.5f;
        unsigned int glassBaseIndex = glassVertices.size() / 10;

        float glassR = 0.85f, glassG = 0.95f, glassB = 1.0f, glassA = 0.15f;

        // Glass faces inward (toward -X direction)
        glassVertices.insert(glassVertices.end(), {glassX, windowBottomY, windowBackZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA,
                                                   glassX, windowBottomY, windowFrontZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA,
                                                   glassX, windowTopY, windowFrontZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA,
                                                   glassX, windowTopY, windowBackZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA});

        glassIndices.insert(glassIndices.end(), {glassBaseIndex + 0, glassBaseIndex + 1, glassBaseIndex + 2,
                                                 glassBaseIndex + 2, glassBaseIndex + 3, glassBaseIndex + 0});

        // === BEVELED WINDOW FRAME - Slopes inward for triangular effect ===
        // Outer edge (at wall surface)
        float frameOuterBackZ = windowBackZ - frameThickness;
        float frameOuterFrontZ = windowFrontZ + frameThickness;
        float frameOuterBottomY = windowBottomY - frameThickness;
        float frameOuterTopY = windowTopY + frameThickness;

        // Inner edge (at back of frame) - SMALLER to create inward slope
        float bevelAmount = frameThickness * 0.6f;
        float frameInnerBackZ = windowBackZ + bevelAmount;
        float frameInnerFrontZ = windowFrontZ - bevelAmount;
        float frameInnerBottomY = windowBottomY + bevelAmount;
        float frameInnerTopY = windowTopY - bevelAmount;

        float frameR = 1.0f, frameG = 1.0f, frameB = 1.0f;
        unsigned int frameBaseIndex = frameVertices.size() / 9;
        float innerX = rightWallX - frameDepth;

        // Front face - outer rectangle (at wall surface)
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   // Inner rectangle at front (glass opening edge)
                                                   rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f});

        // Front face frame (4 beveled strips)
        frameIndices.insert(frameIndices.end(), {
                                                    frameBaseIndex + 0, frameBaseIndex + 1, frameBaseIndex + 5, frameBaseIndex + 5, frameBaseIndex + 4, frameBaseIndex + 0, // Bottom
                                                    frameBaseIndex + 1, frameBaseIndex + 2, frameBaseIndex + 6, frameBaseIndex + 6, frameBaseIndex + 5, frameBaseIndex + 1, // Front
                                                    frameBaseIndex + 2, frameBaseIndex + 3, frameBaseIndex + 7, frameBaseIndex + 7, frameBaseIndex + 6, frameBaseIndex + 2, // Top
                                                    frameBaseIndex + 3, frameBaseIndex + 0, frameBaseIndex + 4, frameBaseIndex + 4, frameBaseIndex + 7, frameBaseIndex + 3  // Back
                                                });

        // Back face - same structure but at innerX (also beveled)
        unsigned int backFaceIndex = frameVertices.size() / 9;
        frameVertices.insert(frameVertices.end(), {innerX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   innerX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   innerX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   innerX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   innerX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   innerX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   innerX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   innerX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f});

        frameIndices.insert(frameIndices.end(), {backFaceIndex + 0, backFaceIndex + 4, backFaceIndex + 5, backFaceIndex + 5, backFaceIndex + 1, backFaceIndex + 0,
                                                 backFaceIndex + 1, backFaceIndex + 5, backFaceIndex + 6, backFaceIndex + 6, backFaceIndex + 2, backFaceIndex + 1,
                                                 backFaceIndex + 2, backFaceIndex + 6, backFaceIndex + 7, backFaceIndex + 7, backFaceIndex + 3, backFaceIndex + 2,
                                                 backFaceIndex + 3, backFaceIndex + 7, backFaceIndex + 4, backFaceIndex + 4, backFaceIndex + 0, backFaceIndex + 3});

        // OUTER SIDES (4 sides - connecting outer edges)
        unsigned int sideIdx = frameVertices.size() / 9;
        // Bottom outer
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   innerX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   innerX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        // Front outer
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   innerX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   innerX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        // Top outer
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   innerX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   innerX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        // Back outer
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   innerX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   innerX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        // INNER BEVELED SIDES (4 sides - connecting inner edges with sloped normals)
        sideIdx = frameVertices.size() / 9;
        // Bottom inner (slopes upward/inward)
        glm::vec3 norm1 = glm::normalize(glm::vec3(-0.2f, 0.8f, 0.0f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   innerX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   innerX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        // Front inner (slopes inward toward room)
        glm::vec3 norm2 = glm::normalize(glm::vec3(-0.2f, 0.0f, -0.8f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   innerX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   innerX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        // Top inner (slopes downward/inward)
        glm::vec3 norm3 = glm::normalize(glm::vec3(-0.2f, -0.8f, 0.0f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   innerX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   innerX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        // Back inner (slopes inward toward room)
        glm::vec3 norm4 = glm::normalize(glm::vec3(-0.2f, 0.0f, 0.8f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   innerX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   innerX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});
    };

    // Create 5 windows in BACK SECTION
    for (int i = 0; i < numBackWindows; i++)
    {
        float windowBackZ = backSectionStart + i * (windowWidth + spacingBetweenWindows);
        float windowFrontZ = windowBackZ + windowWidth;
        createWindow(i + 1, windowBackZ, windowFrontZ);
    }

    // Create 1 window in FRONT SECTION
    for (int i = 0; i < numFrontWindows; i++)
    {
        float windowBackZ = frontSectionStart + i * (windowWidth + spacingBetweenWindows);
        float windowFrontZ = windowBackZ + windowWidth;
        createWindow(numBackWindows + i + 1, windowBackZ, windowFrontZ);
    }

    numGlassIndices = glassIndices.size();
    numFrameIndices = frameIndices.size();

    // Create VAO/VBO/EBO for glass
    glassVAO.Bind();
    glassVBO = new VBO(glassVertices.data(), glassVertices.size() * sizeof(GLfloat));
    glassEBO = new EBO(glassIndices.data(), glassIndices.size() * sizeof(GLuint));
    glassVAO.LinkVBOAttrib(*glassVBO, 0, 3, GL_FLOAT, 10 * sizeof(float), (void *)0);
    glassVAO.LinkVBOAttrib(*glassVBO, 1, 3, GL_FLOAT, 10 * sizeof(float), (void *)(3 * sizeof(float)));
    glassVAO.LinkVBOAttrib(*glassVBO, 2, 3, GL_FLOAT, 10 * sizeof(float), (void *)(6 * sizeof(float)));
    glassVAO.LinkVBOAttrib(*glassVBO, 3, 1, GL_FLOAT, 10 * sizeof(float), (void *)(9 * sizeof(float)));
    glassVAO.Unbind();
    glassVBO->Unbind();
    glassEBO->Unbind();

    // Create VAO/VBO/EBO for frames
    frameVAO.Bind();
    frameVBO = new VBO(frameVertices.data(), frameVertices.size() * sizeof(GLfloat));
    frameEBO = new EBO(frameIndices.data(), frameIndices.size() * sizeof(GLuint));
    frameVAO.LinkVBOAttrib(*frameVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    frameVAO.LinkVBOAttrib(*frameVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    frameVAO.LinkVBOAttrib(*frameVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    frameVAO.Unbind();
    frameVBO->Unbind();
    frameEBO->Unbind();

    std::cout << "✓ RIGHT WALL windows created - 2 distinct sections!" << std::endl;
    std::cout << "  Glass: " << numGlassIndices / 3 << " triangles" << std::endl;
    std::cout << "  Frames: " << numFrameIndices / 3 << " triangles (BEVELED 5cm)" << std::endl;
    std::cout << "===========================================\n"
              << std::endl;
}

void RightWallWindows::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw opaque white frames FIRST
    glUniform1f(glGetUniformLocation(shader.ID, "transparency"), 1.0f);
    frameVAO.Bind();
    glDrawElements(GL_TRIANGLES, numFrameIndices, GL_UNSIGNED_INT, 0);

    // NOW draw transparent glass with proper blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Disable depth writing for glass so background shows through
    glDepthMask(GL_FALSE);

    glUniform1f(glGetUniformLocation(shader.ID, "transparency"), 0.15f); // 15% opacity
    glassVAO.Bind();
    glDrawElements(GL_TRIANGLES, numGlassIndices, GL_UNSIGNED_INT, 0);

    // Restore depth writing and disable blending
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void RightWallWindows::Delete()
{
    glassVAO.Delete();
    if (glassVBO)
    {
        glassVBO->Delete();
        delete glassVBO;
        glassVBO = nullptr;
    }
    if (glassEBO)
    {
        glassEBO->Delete();
        delete glassEBO;
        glassEBO = nullptr;
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
