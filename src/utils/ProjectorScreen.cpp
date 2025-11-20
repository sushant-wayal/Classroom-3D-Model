#include "../../include/ProjectorScreen.h"
#include <iostream>
#include <cmath>

ProjectorScreen::ProjectorScreen(float roomLength, float roomWidth, float roomHeight)
{
    // Initialize pointers
    screenVBO = nullptr;
    screenEBO = nullptr;

    // ROD CALCULATION - SIMPLE AND CORRECT
    // Rod model spans from -8.098977 to 8.098977 = 16.2m in model space
    // Rod is scaled by 0.5 in main.cpp
    // So physical rod width = 16.2 * 0.5 = 8.1m
    float rodModelWidth = 16.2f;                             // Rod's original model width
    float rodScaleInMain = 0.5f;                             // Scale applied in main.cpp
    float rodPhysicalWidth = rodModelWidth * rodScaleInMain; // 8.1m

    // Screen width should be 95% of the physical rod width
    this->screenWidth = rodPhysicalWidth * 0.95f; // 7.695m - properly centered!

    // Green board dimensions for calculating drop height
    float boardHeight = roomHeight * 0.35f;
    float boardBottomY = roomHeight / 2.0f - boardHeight / 2.0f;

    // Rod position - centered horizontally, above the boards
    float boardTopY = roomHeight / 2.0f + boardHeight / 2.0f;
    this->rodY = boardTopY + 0.3f; // 30cm above boards

    // Screen drops to 0.5m BELOW the green board bottom
    float screenBottomY = boardBottomY - 0.5f;
    this->screenMaxHeight = this->rodY - screenBottomY;

    // Z position - in front of the front wall
    float frontWallZ = roomWidth / 2.0f;
    this->screenZ = frontWallZ - 0.20f;

    // Border dimensions (black borders on left, right, and bottom)
    this->borderWidth = 0.10f; // 10cm black border

    // Animation parameters
    this->isDroppedDown = false;
    this->screenExtension = 0.0f;
    this->targetExtension = 0.0f;
    this->animationSpeed = 1.5f;

    std::cout << "\n=== Creating PROJECTOR SCREEN (Interactive) ===" << std::endl;
    std::cout << "Rod model width: " << rodModelWidth << "m" << std::endl;
    std::cout << "Rod scale in main.cpp: " << rodScaleInMain << std::endl;
    std::cout << "Rod physical width: " << rodPhysicalWidth << "m" << std::endl;
    std::cout << "Screen width: " << screenWidth << "m (95% of rod)" << std::endl;
    std::cout << "Screen extends: 0.5m below green boards" << std::endl;
    std::cout << "Max screen height: " << screenMaxHeight << "m" << std::endl;
    std::cout << "Border width: " << borderWidth << "m (black borders on 3 sides)" << std::endl;
    std::cout << "Screen starts rolled up. Press 'P' to toggle!" << std::endl;
    std::cout << "===========================================\n"
              << std::endl;

    UpdateScreenGeometry();
}

void ProjectorScreen::UpdateScreenGeometry()
{
    std::vector<GLfloat> screenVertices;
    std::vector<GLuint> screenIndices;

    // Current screen height based on extension
    float currentHeight = screenMaxHeight * screenExtension;

    // White screen color (matte white surface for projection area)
    float screenR = 0.95f, screenG = 0.95f, screenB = 0.95f;

    // Black border color
    float borderR = 0.00f, borderG = 0.00f, borderB = 0.00f;

    if (currentHeight > 0.01f) // Only create geometry if screen is visible
    {
        // Overall screen dimensions (including borders)
        float totalWidth = screenWidth;
        float leftX = -totalWidth / 2.0f;
        float rightX = totalWidth / 2.0f;
        float topY = rodY;
        float bottomY = rodY - currentHeight;

        // White projection area dimensions (inside the borders)
        float whiteLeftX = leftX + borderWidth;
        float whiteRightX = rightX - borderWidth;
        float whiteTopY = topY; // No border on top (rolled from rod)
        float whiteBottomY = bottomY + borderWidth;

        float frontZ = screenZ;
        float backZ = screenZ + 0.01f; // 1cm thickness

        unsigned int vertexIndex = 0;

        // ===== FRONT FACE =====

        // 1. White projection area (center)
        screenVertices.insert(screenVertices.end(), {whiteLeftX, whiteBottomY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteRightX, whiteBottomY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteRightX, whiteTopY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteLeftX, whiteTopY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {0, 1, 2, 2, 3, 0});
        vertexIndex = 4;

        // 2. Left black border
        screenVertices.insert(screenVertices.end(), {leftX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteLeftX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteLeftX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {leftX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {4, 5, 6, 6, 7, 4});
        vertexIndex = 8;

        // 3. Right black border
        screenVertices.insert(screenVertices.end(), {whiteRightX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteRightX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {8, 9, 10, 10, 11, 8});
        vertexIndex = 12;

        // 4. Bottom black border
        screenVertices.insert(screenVertices.end(), {leftX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, whiteBottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {leftX, whiteBottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {12, 13, 14, 14, 15, 12});
        vertexIndex = 16;

        // ===== BACK FACE (simplified - slightly darker) =====
        float backR = 0.85f, backG = 0.85f, backB = 0.85f;

        screenVertices.insert(screenVertices.end(), {leftX, bottomY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, bottomY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, topY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenVertices.insert(screenVertices.end(), {leftX, topY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenIndices.insert(screenIndices.end(), {16, 18, 17, 18, 16, 19});
        vertexIndex = 20;

        // ===== SIDE EDGES for thickness =====
        // Left edge
        screenIndices.insert(screenIndices.end(), {4, 7, 19, 19, 16, 4});
        // Right edge
        screenIndices.insert(screenIndices.end(), {9, 17, 18, 18, 10, 9});
        // Bottom edge
        screenIndices.insert(screenIndices.end(), {12, 16, 17, 17, 13, 12});
        // Top edge (at rod)
        screenIndices.insert(screenIndices.end(), {7, 10, 18, 18, 19, 7});
    }

    numScreenIndices = screenIndices.size();

    // Delete old buffers if they exist
    if (screenVBO)
    {
        screenVBO->Delete();
        delete screenVBO;
        screenVBO = nullptr; // CRITICAL FIX: Set to nullptr after deletion
    }
    if (screenEBO)
    {
        screenEBO->Delete();
        delete screenEBO;
        screenEBO = nullptr; // CRITICAL FIX: Set to nullptr after deletion
    }

    // Create new buffers only if there's geometry
    if (numScreenIndices > 0)
    {
        screenVAO.Bind();
        screenVBO = new VBO(screenVertices.data(), screenVertices.size() * sizeof(GLfloat));
        screenEBO = new EBO(screenIndices.data(), screenIndices.size() * sizeof(GLuint));
        screenVAO.LinkVBOAttrib(*screenVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
        screenVAO.LinkVBOAttrib(*screenVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
        screenVAO.LinkVBOAttrib(*screenVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
        screenVAO.Unbind();
        screenVBO->Unbind();
        screenEBO->Unbind();
    }
}

void ProjectorScreen::Update(float deltaTime)
{
    // Smoothly animate towards target extension
    if (std::abs(screenExtension - targetExtension) > 0.001f)
    {
        float step = animationSpeed * deltaTime;

        if (screenExtension < targetExtension)
        {
            screenExtension += step;
            if (screenExtension > targetExtension)
                screenExtension = targetExtension;
        }
        else
        {
            screenExtension -= step;
            if (screenExtension < targetExtension)
                screenExtension = targetExtension;
        }

        // Regenerate geometry with new extension
        UpdateScreenGeometry();
    }
}

void ProjectorScreen::ToggleScreen()
{
    isDroppedDown = !isDroppedDown;
    targetExtension = isDroppedDown ? 1.0f : 0.0f;

    std::cout << (isDroppedDown ? "Dropping down" : "Rolling up") << " projector screen..." << std::endl;
}

void ProjectorScreen::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    // Only draw if screen is visible (extension > 0)
    if (screenExtension > 0.001f && numScreenIndices > 0)
    {
        shader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Set transparency to fully opaque
        glUniform1f(glGetUniformLocation(shader.ID, "transparency"), 1.0f);

        screenVAO.Bind();
        glDrawElements(GL_TRIANGLES, numScreenIndices, GL_UNSIGNED_INT, 0);
    }
}

void ProjectorScreen::Delete()
{
    screenVAO.Delete();
    if (screenVBO)
    {
        screenVBO->Delete();
        delete screenVBO;
        screenVBO = nullptr;
    }
    if (screenEBO)
    {
        screenEBO->Delete();
        delete screenEBO;
        screenEBO = nullptr;
    }
}
