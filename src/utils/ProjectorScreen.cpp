#include "../../include/ProjectorScreen.h"
#include <iostream>
#include <cmath>

ProjectorScreen::ProjectorScreen(float roomLength, float roomWidth, float roomHeight)
{
    screenVBO = nullptr;
    screenEBO = nullptr;

    float rodModelWidth = 16.2f;                             
    float rodScaleInMain = 0.5f;
    float rodPhysicalWidth = rodModelWidth * rodScaleInMain;

    this->screenWidth = rodPhysicalWidth * 0.95f; 

    float boardHeight = roomHeight * 0.35f;
    float boardBottomY = roomHeight / 2.0f - boardHeight / 2.0f;

    float boardTopY = roomHeight / 2.0f + boardHeight / 2.0f;
    this->rodY = boardTopY + 0.3f;

    float screenBottomY = boardBottomY - 0.5f;
    this->screenMaxHeight = this->rodY - screenBottomY;

    float frontWallZ = roomWidth / 2.0f;
    this->screenZ = frontWallZ - 0.20f;

    this->borderWidth = 0.10f; 

    this->isDroppedDown = false;
    this->screenExtension = 0.0f;
    this->targetExtension = 0.0f;
    this->animationSpeed = 1.5f;

    UpdateScreenGeometry();
}

void ProjectorScreen::UpdateScreenGeometry()
{
    std::vector<GLfloat> screenVertices;
    std::vector<GLuint> screenIndices;

    float currentHeight = screenMaxHeight * screenExtension;

    float screenR = 0.95f, screenG = 0.95f, screenB = 0.95f;

    float borderR = 0.00f, borderG = 0.00f, borderB = 0.00f;

    if (currentHeight > 0.01f) 
    {
        float totalWidth = screenWidth;
        float leftX = -totalWidth / 2.0f;
        float rightX = totalWidth / 2.0f;
        float topY = rodY;
        float bottomY = rodY - currentHeight;

        float whiteLeftX = leftX + borderWidth;
        float whiteRightX = rightX - borderWidth;
        float whiteTopY = topY;
        float whiteBottomY = bottomY + borderWidth;

        float frontZ = screenZ;
        float backZ = screenZ + 0.01f; 

        unsigned int vertexIndex = 0;

        screenVertices.insert(screenVertices.end(), {whiteLeftX, whiteBottomY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteRightX, whiteBottomY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteRightX, whiteTopY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteLeftX, whiteTopY, frontZ, screenR, screenG, screenB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {0, 1, 2, 2, 3, 0});
        vertexIndex = 4;

        screenVertices.insert(screenVertices.end(), {leftX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteLeftX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteLeftX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {leftX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {4, 5, 6, 6, 7, 4});
        vertexIndex = 8;

        screenVertices.insert(screenVertices.end(), {whiteRightX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {whiteRightX, whiteTopY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {8, 9, 10, 10, 11, 8});
        vertexIndex = 12;

        screenVertices.insert(screenVertices.end(), {leftX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, bottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, whiteBottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenVertices.insert(screenVertices.end(), {leftX, whiteBottomY, frontZ, borderR, borderG, borderB, 0.0f, 0.0f, -1.0f});
        screenIndices.insert(screenIndices.end(), {12, 13, 14, 14, 15, 12});
        vertexIndex = 16;

        float backR = 0.85f, backG = 0.85f, backB = 0.85f;

        screenVertices.insert(screenVertices.end(), {leftX, bottomY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, bottomY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenVertices.insert(screenVertices.end(), {rightX, topY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenVertices.insert(screenVertices.end(), {leftX, topY, backZ, backR, backG, backB, 0.0f, 0.0f, 1.0f});
        screenIndices.insert(screenIndices.end(), {16, 18, 17, 18, 16, 19});
        vertexIndex = 20;

        screenIndices.insert(screenIndices.end(), {4, 7, 19, 19, 16, 4});
       
        screenIndices.insert(screenIndices.end(), {9, 17, 18, 18, 10, 9});
      
        screenIndices.insert(screenIndices.end(), {12, 16, 17, 17, 13, 12});
        
        screenIndices.insert(screenIndices.end(), {7, 10, 18, 18, 19, 7});
    }

    numScreenIndices = screenIndices.size();

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

        UpdateScreenGeometry();
    }
}

void ProjectorScreen::ToggleScreen()
{
    isDroppedDown = !isDroppedDown;
    targetExtension = isDroppedDown ? 1.0f : 0.0f;
}

void ProjectorScreen::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    if (screenExtension > 0.001f && numScreenIndices > 0)
    {
        shader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

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
