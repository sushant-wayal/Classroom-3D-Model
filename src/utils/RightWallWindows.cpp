#include "../../include/RightWallWindows.h"
#include <iostream>

RightWallWindows::RightWallWindows(float roomLength, float roomWidth, float roomHeight)
{
    glassVBO = nullptr;
    glassEBO = nullptr;
    frameVBO = nullptr;
    frameEBO = nullptr;

    this->windowHeight = roomHeight * 0.20f; 
    this->windowWidth = 2.7875f;
    this->frameThickness = 0.05f;

    float frameDepth = 0.25f; 

    float rightWallX = roomLength / 2.0f;

    float topMargin = 0.30f; 
    float windowTopY = roomHeight - topMargin;
    float windowBottomY = windowTopY - windowHeight;

    float backMargin = 0.5f;            
    float frontMargin = 0.5f;           
    float spacingBetweenWindows = 0.5f; 

    int numBackWindows = 5;
    float backSectionStart = -roomWidth / 2.0f + backMargin;
    float backSectionLength = numBackWindows * windowWidth + (numBackWindows - 1) * spacingBetweenWindows;

    int numFrontWindows = 1;
    float frontSectionEnd = roomWidth / 2.0f - frontMargin;
    float frontSectionLength = numFrontWindows * windowWidth;
    float frontSectionStart = frontSectionEnd - frontSectionLength;

    float gapBetweenSections = frontSectionStart - (backSectionStart + backSectionLength);

    std::vector<GLfloat> glassVertices;
    std::vector<GLuint> glassIndices;
    std::vector<GLfloat> frameVertices;
    std::vector<GLuint> frameIndices;

    auto createWindow = [&](int windowNum, float windowBackZ, float windowFrontZ)
    {

        float glassX = rightWallX - frameDepth * 0.5f;
        unsigned int glassBaseIndex = glassVertices.size() / 10;

        float glassR = 0.85f, glassG = 0.95f, glassB = 1.0f, glassA = 0.15f;

        glassVertices.insert(glassVertices.end(), {glassX, windowBottomY, windowBackZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA,
                                                   glassX, windowBottomY, windowFrontZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA,
                                                   glassX, windowTopY, windowFrontZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA,
                                                   glassX, windowTopY, windowBackZ, glassR, glassG, glassB, -1.0f, 0.0f, 0.0f, glassA});

        glassIndices.insert(glassIndices.end(), {glassBaseIndex + 0, glassBaseIndex + 1, glassBaseIndex + 2,
                                                 glassBaseIndex + 2, glassBaseIndex + 3, glassBaseIndex + 0});

        float frameOuterBackZ = windowBackZ - frameThickness;
        float frameOuterFrontZ = windowFrontZ + frameThickness;
        float frameOuterBottomY = windowBottomY - frameThickness;
        float frameOuterTopY = windowTopY + frameThickness;

        float bevelAmount = frameThickness * 0.6f;
        float frameInnerBackZ = windowBackZ + bevelAmount;
        float frameInnerFrontZ = windowFrontZ - bevelAmount;
        float frameInnerBottomY = windowBottomY + bevelAmount;
        float frameInnerTopY = windowTopY - bevelAmount;

        float frameR = 1.0f, frameG = 1.0f, frameB = 1.0f;
        unsigned int frameBaseIndex = frameVertices.size() / 9;
        float innerX = rightWallX - frameDepth;

        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f,
                                                   rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, 1.0f, 0.0f, 0.0f});

        frameIndices.insert(frameIndices.end(), {
                                                    frameBaseIndex + 0, frameBaseIndex + 1, frameBaseIndex + 5, frameBaseIndex + 5, frameBaseIndex + 4, frameBaseIndex + 0,
                                                    frameBaseIndex + 1, frameBaseIndex + 2, frameBaseIndex + 6, frameBaseIndex + 6, frameBaseIndex + 5, frameBaseIndex + 1,
                                                    frameBaseIndex + 2, frameBaseIndex + 3, frameBaseIndex + 7, frameBaseIndex + 7, frameBaseIndex + 6, frameBaseIndex + 2,
                                                    frameBaseIndex + 3, frameBaseIndex + 0, frameBaseIndex + 4, frameBaseIndex + 4, frameBaseIndex + 7, frameBaseIndex + 3
                                                });

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

        unsigned int sideIdx = frameVertices.size() / 9;
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   innerX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f,
                                                   innerX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, -1.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   innerX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f,
                                                   innerX, frameOuterBottomY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 0.0f, 1.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   rightWallX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   innerX, frameOuterTopY, frameOuterFrontZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f,
                                                   innerX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 1.0f, 0.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        frameVertices.insert(frameVertices.end(), {rightWallX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   rightWallX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   innerX, frameOuterTopY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f,
                                                   innerX, frameOuterBottomY, frameOuterBackZ, frameR, frameG, frameB, 0.0f, 0.0f, -1.0f});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        glm::vec3 norm1 = glm::normalize(glm::vec3(-0.2f, 0.8f, 0.0f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   innerX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z,
                                                   innerX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm1.x, norm1.y, norm1.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        glm::vec3 norm2 = glm::normalize(glm::vec3(-0.2f, 0.0f, -0.8f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   innerX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z,
                                                   innerX, frameInnerBottomY, frameInnerFrontZ, frameR, frameG, frameB, norm2.x, norm2.y, norm2.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 2, sideIdx + 1, sideIdx + 2, sideIdx + 0, sideIdx + 3});

        sideIdx = frameVertices.size() / 9;
        glm::vec3 norm3 = glm::normalize(glm::vec3(-0.2f, -0.8f, 0.0f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   rightWallX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   innerX, frameInnerTopY, frameInnerFrontZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z,
                                                   innerX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm3.x, norm3.y, norm3.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});

        sideIdx = frameVertices.size() / 9;
        glm::vec3 norm4 = glm::normalize(glm::vec3(-0.2f, 0.0f, 0.8f));
        frameVertices.insert(frameVertices.end(), {rightWallX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   rightWallX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   innerX, frameInnerTopY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z,
                                                   innerX, frameInnerBottomY, frameInnerBackZ, frameR, frameG, frameB, norm4.x, norm4.y, norm4.z});
        frameIndices.insert(frameIndices.end(), {sideIdx + 0, sideIdx + 1, sideIdx + 2, sideIdx + 2, sideIdx + 3, sideIdx + 0});
    };

    for (int i = 0; i < numBackWindows; i++)
    {
        float windowBackZ = backSectionStart + i * (windowWidth + spacingBetweenWindows);
        float windowFrontZ = windowBackZ + windowWidth;
        createWindow(i + 1, windowBackZ, windowFrontZ);
    }

    for (int i = 0; i < numFrontWindows; i++)
    {
        float windowBackZ = frontSectionStart + i * (windowWidth + spacingBetweenWindows);
        float windowFrontZ = windowBackZ + windowWidth;
        createWindow(numBackWindows + i + 1, windowBackZ, windowFrontZ);
    }

    numGlassIndices = glassIndices.size();
    numFrameIndices = frameIndices.size();

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(GL_FALSE);

    glUniform1f(glGetUniformLocation(shader.ID, "transparency"), 0.15f); // 15% opacity
    glassVAO.Bind();
    glDrawElements(GL_TRIANGLES, numGlassIndices, GL_UNSIGNED_INT, 0);

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
