#include "../../include/Windows.h"
#include <iostream>

Windows::Windows(float roomLength, float roomWidth, float roomHeight, int numWindows)
{
    this->numWindows = numWindows;

    // Initialize pointers
    glassVBO = nullptr;
    glassEBO = nullptr;
    frameVBO = nullptr;
    frameEBO = nullptr;

    // Window dimensions - RECTANGULAR (wider and shorter)
    this->windowHeight = roomHeight * 0.20f; // 20% of room height (1.4m for 7m ceiling) - SHORTER

    // EVEN THINNER frames - more elegant and uniform on all 4 sides
    this->frameThickness = 0.05f; // 5cm thick frames (REDUCED from 10cm) - THINNER!

    // Calculate window width with spacing - Small margins to keep frames inside room
    float sideMargin = frameThickness + 0.05f; // Frame thickness + 5cm buffer = 10cm margin on each side
    float spacingBetweenWindows = 0.50f;       // 50cm gap between windows
    float totalAvailableLength = roomLength - (2.0f * sideMargin);
    float totalSpacing = spacingBetweenWindows * (numWindows - 1);
    this->windowWidth = (totalAvailableLength - totalSpacing) / numWindows;

    float frameDepth = 0.25f; // Frame extends 25cm from wall

    // Back wall position
    float backWallZ = -roomWidth / 2.0f;

    // Windows positioned at TOP with REDUCED margin
    float topMargin = 0.30f; // 30cm from ceiling (reduced from 60cm)
    float windowTopY = roomHeight - topMargin;
    float windowBottomY = windowTopY - windowHeight;

    // Starting X position with margin
    float startX = -roomLength / 2.0f + sideMargin;

    std::vector<GLfloat> glassVertices;
    std::vector<GLuint> glassIndices;
    std::vector<GLfloat> frameVertices;
    std::vector<GLuint> frameIndices;

    // Create each window separately
    for (int i = 0; i < numWindows; i++)
    {
        // Calculate THIS window's position (with spacing)
        float windowLeftX = startX + i * (windowWidth + spacingBetweenWindows);
        float windowRightX = windowLeftX + windowWidth;

        // === GLASS - centered in opening ===
        float glassZ = backWallZ + frameDepth * 0.5f;
        unsigned int glassBaseIndex = glassVertices.size() / 10;

        float glassR = 0.85f, glassG = 0.95f, glassB = 1.0f, glassA = 0.15f;

        glassVertices.insert(glassVertices.end(), {windowLeftX, windowBottomY, glassZ, glassR, glassG, glassB, 0.0f, 0.0f, 1.0f, glassA,
                                                   windowRightX, windowBottomY, glassZ, glassR, glassG, glassB, 0.0f, 0.0f, 1.0f, glassA,
                                                   windowRightX, windowTopY, glassZ, glassR, glassG, glassB, 0.0f, 0.0f, 1.0f, glassA,
                                                   windowLeftX, windowTopY, glassZ, glassR, glassG, glassB, 0.0f, 0.0f, 1.0f, glassA});

        glassIndices.insert(glassIndices.end(), {glassBaseIndex + 0, glassBaseIndex + 1, glassBaseIndex + 2,
                                                 glassBaseIndex + 2, glassBaseIndex + 3, glassBaseIndex + 0});

        // === BEVELED WINDOW FRAME - Slopes inward for triangular effect ===
        // Outer edge (at wall surface)
        float frameOuterLeftX = windowLeftX - frameThickness;
        float frameOuterRightX = windowRightX + frameThickness;
        float frameOuterBottomY = windowBottomY - frameThickness;
        float frameOuterTopY = windowTopY + frameThickness;

        float bevelAmount = frameThickness * 0.6f; 
        float frameInnerLeftX = windowLeftX + bevelAmount;
        float frameInnerRightX = windowRightX - bevelAmount;
        float frameInnerBottomY = windowBottomY + bevelAmount;
        float frameInnerTopY = windowTopY - bevelAmount;

        float frameR = 1.0f, frameG = 1.0f, frameB = 1.0f;
        unsigned int frameBaseIndex = frameVertices.size() / 9;
        float backZ = backWallZ + frameDepth;

        frameVertices.insert(frameVertices.end(), {frameOuterLeftX, frameOuterBottomY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   frameOuterRightX, frameOuterBottomY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   frameOuterRightX, frameOuterTopY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   frameOuterLeftX, frameOuterTopY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   
                                                   frameInnerLeftX, frameInnerBottomY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   frameInnerRightX, frameInnerBottomY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   frameInnerRightX, frameInnerTopY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   frameInnerLeftX, frameInnerTopY, backWallZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f});

        frameIndices.insert(frameIndices.end(), {
                                                    frameBaseIndex + 0, frameBaseIndex + 1, frameBaseIndex + 5, frameBaseIndex + 5, frameBaseIndex + 4, frameBaseIndex + 0, // Bottom
                                                    frameBaseIndex + 1, frameBaseIndex + 2, frameBaseIndex + 6, frameBaseIndex + 6, frameBaseIndex + 5, frameBaseIndex + 1, // Right
                                                    frameBaseIndex + 2, frameBaseIndex + 3, frameBaseIndex + 7, frameBaseIndex + 7, frameBaseIndex + 6, frameBaseIndex + 2, // Top
                                                    frameBaseIndex + 3, frameBaseIndex + 0, frameBaseIndex + 4, frameBaseIndex + 4, frameBaseIndex + 7, frameBaseIndex + 3  // Left
                                                });

        unsigned int backFaceIndex = frameVertices.size() / 9;
        frameVertices.insert(frameVertices.end(), {frameOuterLeftX, frameOuterBottomY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   frameOuterRightX, frameOuterBottomY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   frameOuterRightX, frameOuterTopY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   frameOuterLeftX, frameOuterTopY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   frameInnerLeftX, frameInnerBottomY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   frameInnerRightX, frameInnerBottomY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   frameInnerRightX, frameInnerTopY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   frameInnerLeftX, frameInnerTopY, backZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f});

        frameIndices.insert(frameIndices.end(), {backFaceIndex + 0, backFaceIndex + 4, backFaceIndex + 5, backFaceIndex + 5, backFaceIndex + 1, backFaceIndex + 0,
                                                 backFaceIndex + 1, backFaceIndex + 5, backFaceIndex + 6, backFaceIndex + 6, backFaceIndex + 2, backFaceIndex + 1,
                                                 backFaceIndex + 2, backFaceIndex + 6, backFaceIndex + 7, backFaceIndex + 7, backFaceIndex + 3, backFaceIndex + 2,
                                                 backFaceIndex + 3, backFaceIndex + 7, backFaceIndex + 4, backFaceIndex + 4, backFaceIndex + 0, backFaceIndex + 3});

        unsigned int sideIdx = frameVertices.size() / 9;
       
        frameVertices.insert(frameVertices.end(), {frameOuterLeftX, frameOuterBottomY, backWallZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   frameOuterRightX, frameOuterBottomY, backWallZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   frameOuterRightX, frameOuterBottomY, backZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   frameOuterLeftX, frameOuterBottomY, backZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
      
        frameVertices.insert(frameVertices.end(), {frameOuterRightX, frameOuterBottomY, backWallZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   frameOuterRightX, frameOuterTopY, backWallZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   frameOuterRightX, frameOuterTopY, backZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   frameOuterRightX, frameOuterBottomY, backZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        
        frameVertices.insert(frameVertices.end(), {frameOuterLeftX, frameOuterTopY, backWallZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   frameOuterRightX, frameOuterTopY, backWallZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   frameOuterRightX, frameOuterTopY, backZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   frameOuterLeftX, frameOuterTopY, backZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        // Left outer
        frameVertices.insert(frameVertices.end(), {frameOuterLeftX, frameOuterBottomY, backWallZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   frameOuterLeftX, frameOuterTopY, backWallZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   frameOuterLeftX, frameOuterTopY, backZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f,
                                                   frameOuterLeftX, frameOuterBottomY, backZ, frameR, frameG, frameB, -1.0f, 0.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        // INNER BEVELED SIDES (4 sides - connecting inner edges with sloped normals)
        sideIdx = frameVertices.size() / 9;
        // Bottom inner (slopes upward/inward)
        glm::vec3 norm1 = glm::normalize(glm::vec3(0.0f, 0.8f, 0.2f));
        frameVertices.insert(frameVertices.end(), {frameInnerLeftX, frameInnerBottomY, backWallZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   frameInnerRightX, frameInnerBottomY, backWallZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   frameInnerRightX, frameInnerBottomY, backZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   frameInnerLeftX, frameInnerBottomY, backZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        // Right inner (slopes leftward/inward)
        glm::vec3 norm2 = glm::normalize(glm::vec3(-0.8f, 0.0f, 0.2f));
        frameVertices.insert(frameVertices.end(), {frameInnerRightX, frameInnerBottomY, backWallZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   frameInnerRightX, frameInnerTopY, backWallZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   frameInnerRightX, frameInnerTopY, backZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   frameInnerRightX, frameInnerBottomY, backZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        // Top inner (slopes downward/inward)
        glm::vec3 norm3 = glm::normalize(glm::vec3(0.0f, -0.8f, 0.2f));
        frameVertices.insert(frameVertices.end(), {frameInnerLeftX, frameInnerTopY, backWallZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   frameInnerRightX, frameInnerTopY, backWallZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   frameInnerRightX, frameInnerTopY, backZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   frameInnerLeftX, frameInnerTopY, backZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        // Left inner (slopes rightward/inward)
        glm::vec3 norm4 = glm::normalize(glm::vec3(0.8f, 0.0f, 0.2f));
        frameVertices.insert(frameVertices.end(), {frameInnerLeftX, frameInnerBottomY, backWallZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   frameInnerLeftX, frameInnerTopY, backWallZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   frameInnerLeftX, frameInnerTopY, backZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   frameInnerLeftX, frameInnerBottomY, backZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});
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
}

void Windows::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
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

void Windows::Delete()
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
