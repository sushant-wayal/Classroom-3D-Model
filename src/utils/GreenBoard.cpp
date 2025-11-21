#include "../../include/GreenBoard.h"
#include <iostream>
#include <cmath>

GreenBoard::GreenBoard(float roomLength, float roomWidth, float roomHeight)
{
    // Initialize pointers
    boardVBO = nullptr;
    boardEBO = nullptr;
    frameVBO = nullptr;
    frameEBO = nullptr;

    // Two boards side-by-side, centered on front wall
    // Reduced board width by 25% (from 40% to 30% of room length)
    this->boardWidth = roomLength * 0.30f;  // Each board width (REDUCED)
    this->boardHeight = roomHeight * 0.35f; // Board height
    this->frameThickness = 0.08f;           // 8cm thick metallic frame

    float frontWallZ = roomWidth / 2.0f;
    float boardDepth = 0.05f; // Boards extend 5cm from wall
    float frameDepth = 0.12f; // Frames extend 12cm from wall (more prominent)

    // Calculate positions - centered horizontally and vertically
    // Gap between boards = 2x frame thickness so frames don't overlap
    float gapBetweenBoards = 2.0f * frameThickness;       // Gap to prevent frame overlap (16cm)
    float totalWidth = 2 * boardWidth + gapBetweenBoards; // Total width of both boards + gap
    float centerY = roomHeight / 2.0f;                    // Center vertically

    float leftBoardLeftX = -totalWidth / 2.0f;
    float leftBoardRightX = leftBoardLeftX + boardWidth;
    float rightBoardLeftX = leftBoardRightX + gapBetweenBoards; // Add gap
    float rightBoardRightX = rightBoardLeftX + boardWidth;

    float boardBottomY = centerY - boardHeight / 2.0f;
    float boardTopY = centerY + boardHeight / 2.0f;

    std::vector<GLfloat> boardVertices;
    std::vector<GLuint> boardIndices;
    std::vector<GLfloat> frameVertices;
    std::vector<GLuint> frameIndices;

    // DARKER green board color (much darker matte green)
    float boardR = 0.05f, boardG = 0.15f, boardB = 0.05f; // Very dark green (Option 1)

    // Create rounded corners using segments
    int cornerSegments = 8;     // Number of segments for rounded corners
    float cornerRadius = 0.15f; // 15cm corner radius

    auto createRoundedBoard = [&](float leftX, float rightX, float bottomY, float topY, float z)
    {
        unsigned int baseIndex = boardVertices.size() / 9;
        std::vector<glm::vec3> vertices;

        // Calculate corner centers
        float innerLeft = leftX + cornerRadius;
        float innerRight = rightX - cornerRadius;
        float innerBottom = bottomY + cornerRadius;
        float innerTop = topY - cornerRadius;

        // Bottom-left corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(180.0f + i * 90.0f / cornerSegments);
            float x = innerLeft + cornerRadius * cos(angle);
            float y = innerBottom + cornerRadius * sin(angle);
            vertices.push_back(glm::vec3(x, y, z));
        }

        // Bottom-right corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(270.0f + i * 90.0f / cornerSegments);
            float x = innerRight + cornerRadius * cos(angle);
            float y = innerBottom + cornerRadius * sin(angle);
            vertices.push_back(glm::vec3(x, y, z));
        }

        // Top-right corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(0.0f + i * 90.0f / cornerSegments);
            float x = innerRight + cornerRadius * cos(angle);
            float y = innerTop + cornerRadius * sin(angle);
            vertices.push_back(glm::vec3(x, y, z));
        }

        // Top-left corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(90.0f + i * 90.0f / cornerSegments);
            float x = innerLeft + cornerRadius * cos(angle);
            float y = innerTop + cornerRadius * sin(angle);
            vertices.push_back(glm::vec3(x, y, z));
        }

        // Center point for triangle fan
        glm::vec3 center((leftX + rightX) / 2.0f, (bottomY + topY) / 2.0f, z);
        boardVertices.insert(boardVertices.end(), {center.x, center.y, center.z, boardR, boardG, boardB, 0.0f, 0.0f, -1.0f});
        unsigned int centerIdx = baseIndex;

        // Add all perimeter vertices
        for (const auto &v : vertices)
        {
            boardVertices.insert(boardVertices.end(), {v.x, v.y, v.z, boardR, boardG, boardB, 0.0f, 0.0f, -1.0f});
        }

        // Create triangle fan from center
        for (unsigned int i = 0; i < vertices.size(); i++)
        {
            boardIndices.push_back(centerIdx);
            boardIndices.push_back(baseIndex + 1 + i);
            boardIndices.push_back(baseIndex + 1 + ((i + 1) % vertices.size()));
        }
    };

    // Create two boards with rounded corners
    float boardZ = frontWallZ - boardDepth;
    createRoundedBoard(leftBoardLeftX, leftBoardRightX, boardBottomY, boardTopY, boardZ);
    createRoundedBoard(rightBoardLeftX, rightBoardRightX, boardBottomY, boardTopY, boardZ);

    // Shiny grayish metallic frame color
    float frameR = 0.65f, frameG = 0.68f, frameB = 0.70f; // Grayish metallic

    auto createRoundedFrame = [&](float leftX, float rightX, float bottomY, float topY)
    {
        // Calculate corner centers for rounded frame
        float outerCornerRadius = cornerRadius + frameThickness;
        float innerCornerRadius = cornerRadius;

        float frameBackZ = frontWallZ - frameDepth;
        float frameFrontZ = frontWallZ - boardDepth + 0.01f;

        // Create rounded frame with proper rounded corners
        std::vector<glm::vec3> outerCornerVertices;
        std::vector<glm::vec3> innerCornerVertices;

        // Calculate outer and inner corner positions
        float outerLeft = leftX - frameThickness;
        float outerRight = rightX + frameThickness;
        float outerBottom = bottomY - frameThickness;
        float outerTop = topY + frameThickness;

        float outerInnerLeft = outerLeft + outerCornerRadius;
        float outerInnerRight = outerRight - outerCornerRadius;
        float outerInnerBottom = outerBottom + outerCornerRadius;
        float outerInnerTop = outerTop - outerCornerRadius;

        float innerInnerLeft = leftX + innerCornerRadius;
        float innerInnerRight = rightX - innerCornerRadius;
        float innerInnerBottom = bottomY + innerCornerRadius;
        float innerInnerTop = topY - innerCornerRadius;

        // Generate outer rounded perimeter
        // Bottom-left outer corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(180.0f + i * 90.0f / cornerSegments);
            float x = outerInnerLeft + outerCornerRadius * cos(angle);
            float y = outerInnerBottom + outerCornerRadius * sin(angle);
            outerCornerVertices.push_back(glm::vec3(x, y, 0));
        }
        // Bottom-right outer corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(270.0f + i * 90.0f / cornerSegments);
            float x = outerInnerRight + outerCornerRadius * cos(angle);
            float y = outerInnerBottom + outerCornerRadius * sin(angle);
            outerCornerVertices.push_back(glm::vec3(x, y, 0));
        }
        // Top-right outer corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(0.0f + i * 90.0f / cornerSegments);
            float x = outerInnerRight + outerCornerRadius * cos(angle);
            float y = outerInnerTop + outerCornerRadius * sin(angle);
            outerCornerVertices.push_back(glm::vec3(x, y, 0));
        }
        // Top-left outer corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(90.0f + i * 90.0f / cornerSegments);
            float x = outerInnerLeft + outerCornerRadius * cos(angle);
            float y = outerInnerTop + outerCornerRadius * sin(angle);
            outerCornerVertices.push_back(glm::vec3(x, y, 0));
        }

        // Generate inner rounded perimeter
        // Bottom-left inner corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(180.0f + i * 90.0f / cornerSegments);
            float x = innerInnerLeft + innerCornerRadius * cos(angle);
            float y = innerInnerBottom + innerCornerRadius * sin(angle);
            innerCornerVertices.push_back(glm::vec3(x, y, 0));
        }
        // Bottom-right inner corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(270.0f + i * 90.0f / cornerSegments);
            float x = innerInnerRight + innerCornerRadius * cos(angle);
            float y = innerInnerBottom + innerCornerRadius * sin(angle);
            innerCornerVertices.push_back(glm::vec3(x, y, 0));
        }
        // Top-right inner corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(0.0f + i * 90.0f / cornerSegments);
            float x = innerInnerRight + innerCornerRadius * cos(angle);
            float y = innerInnerTop + innerCornerRadius * sin(angle);
            innerCornerVertices.push_back(glm::vec3(x, y, 0));
        }
        // Top-left inner corner
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = glm::radians(90.0f + i * 90.0f / cornerSegments);
            float x = innerInnerLeft + innerCornerRadius * cos(angle);
            float y = innerInnerTop + innerCornerRadius * sin(angle);
            innerCornerVertices.push_back(glm::vec3(x, y, 0));
        }

        // Create front face with rounded corners
        unsigned int baseIdx = frameVertices.size() / 9;

        // Add outer vertices
        for (const auto &v : outerCornerVertices)
        {
            frameVertices.insert(frameVertices.end(), {v.x, v.y, frameFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f});
        }

        // Add inner vertices
        unsigned int innerStartIdx = frameVertices.size() / 9;
        for (const auto &v : innerCornerVertices)
        {
            frameVertices.insert(frameVertices.end(), {v.x, v.y, frameFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f});
        }

        // Create frame strip by connecting outer and inner loops
        unsigned int outerCount = outerCornerVertices.size();
        unsigned int innerCount = innerCornerVertices.size();
        for (unsigned int i = 0; i < outerCount; i++)
        {
            unsigned int outerCur = baseIdx + i;
            unsigned int outerNext = baseIdx + ((i + 1) % outerCount);
            unsigned int innerCur = innerStartIdx + i;
            unsigned int innerNext = innerStartIdx + ((i + 1) % innerCount);

            // Two triangles forming a quad
            frameIndices.push_back(outerCur);
            frameIndices.push_back(outerNext);
            frameIndices.push_back(innerCur);

            frameIndices.push_back(innerCur);
            frameIndices.push_back(outerNext);
            frameIndices.push_back(innerNext);
        }

        // Create back face
        unsigned int backBaseIdx = frameVertices.size() / 9;
        for (const auto &v : outerCornerVertices)
        {
            frameVertices.insert(frameVertices.end(), {v.x, v.y, frameBackZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f});
        }
        unsigned int backInnerStartIdx = frameVertices.size() / 9;
        for (const auto &v : innerCornerVertices)
        {
            frameVertices.insert(frameVertices.end(), {v.x, v.y, frameBackZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f});
        }

        // Create back frame strip
        for (unsigned int i = 0; i < outerCount; i++)
        {
            unsigned int outerCur = backBaseIdx + i;
            unsigned int outerNext = backBaseIdx + ((i + 1) % outerCount);
            unsigned int innerCur = backInnerStartIdx + i;
            unsigned int innerNext = backInnerStartIdx + ((i + 1) % innerCount);

            frameIndices.push_back(outerCur);
            frameIndices.push_back(innerCur);
            frameIndices.push_back(outerNext);

            frameIndices.push_back(innerCur);
            frameIndices.push_back(innerNext);
            frameIndices.push_back(outerNext);
        }

        // Connect front and back with side strips (outer edge)
        for (unsigned int i = 0; i < outerCount; i++)
        {
            unsigned int frontCur = baseIdx + i;
            unsigned int frontNext = baseIdx + ((i + 1) % outerCount);
            unsigned int backCur = backBaseIdx + i;
            unsigned int backNext = backBaseIdx + ((i + 1) % outerCount);

            frameIndices.push_back(frontCur);
            frameIndices.push_back(frontNext);
            frameIndices.push_back(backCur);

            frameIndices.push_back(backCur);
            frameIndices.push_back(frontNext);
            frameIndices.push_back(backNext);
        }

        // Connect front and back with side strips (inner edge)
        for (unsigned int i = 0; i < innerCount; i++)
        {
            unsigned int frontCur = innerStartIdx + i;
            unsigned int frontNext = innerStartIdx + ((i + 1) % innerCount);
            unsigned int backCur = backInnerStartIdx + i;
            unsigned int backNext = backInnerStartIdx + ((i + 1) % innerCount);

            frameIndices.push_back(frontCur);
            frameIndices.push_back(backCur);
            frameIndices.push_back(frontNext);

            frameIndices.push_back(backCur);
            frameIndices.push_back(backNext);
            frameIndices.push_back(frontNext);
        }
    };

    // Create frames for both boards
    createRoundedFrame(leftBoardLeftX, leftBoardRightX, boardBottomY, boardTopY);
    createRoundedFrame(rightBoardLeftX, rightBoardRightX, boardBottomY, boardTopY);

    numBoardIndices = boardIndices.size();
    numFrameIndices = frameIndices.size();

    // Create VAO/VBO/EBO for boards
    boardVAO.Bind();
    boardVBO = new VBO(boardVertices.data(), boardVertices.size() * sizeof(GLfloat));
    boardEBO = new EBO(boardIndices.data(), boardIndices.size() * sizeof(GLuint));
    boardVAO.LinkVBOAttrib(*boardVBO, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    boardVAO.LinkVBOAttrib(*boardVBO, 1, 3, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    boardVAO.LinkVBOAttrib(*boardVBO, 2, 3, GL_FLOAT, 9 * sizeof(float), (void *)(6 * sizeof(float)));
    boardVAO.Unbind();
    boardVBO->Unbind();
    boardEBO->Unbind();

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

void GreenBoard::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw shiny metallic frames FIRST
    glUniform1f(glGetUniformLocation(shader.ID, "transparency"), 1.0f);
    frameVAO.Bind();
    glDrawElements(GL_TRIANGLES, numFrameIndices, GL_UNSIGNED_INT, 0);

    // Draw rough green boards
    boardVAO.Bind();
    glDrawElements(GL_TRIANGLES, numBoardIndices, GL_UNSIGNED_INT, 0);
}

void GreenBoard::Delete()
{
    boardVAO.Delete();
    if (boardVBO)
    {
        boardVBO->Delete();
        delete boardVBO;
        boardVBO = nullptr;
    }
    if (boardEBO)
    {
        boardEBO->Delete();
        delete boardEBO;
        boardEBO = nullptr;
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
