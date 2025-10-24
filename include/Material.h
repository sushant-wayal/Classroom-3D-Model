#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "Texture.h"

enum MaterialType
{
    MATERIAL_WOOD,
    MATERIAL_METAL,
    MATERIAL_PLASTIC,
    MATERIAL_FABRIC
};

struct Material
{
    MaterialType type;
    glm::vec3 baseColor;
    float metallic;
    float roughness;
    float specularStrength;
    Texture *diffuseTexture;
    bool useTexture;

    // Constructor for textured material
    Material(MaterialType matType, const char *texturePath)
    {
        type = matType;
        diffuseTexture = new Texture(texturePath);
        useTexture = true;
        setupMaterialProperties();
    }

    // Constructor for solid color material
    Material(MaterialType matType, glm::vec3 color)
    {
        type = matType;
        baseColor = color;
        diffuseTexture = nullptr;
        useTexture = false;
        setupMaterialProperties();
    }

    // Default constructor
    Material()
    {
        type = MATERIAL_WOOD;
        baseColor = glm::vec3(0.8f, 0.6f, 0.4f);
        diffuseTexture = nullptr;
        useTexture = false;
        setupMaterialProperties();
    }

    void setupMaterialProperties()
    {
        switch (type)
        {
        case MATERIAL_WOOD:
            if (!useTexture)
                baseColor = glm::vec3(0.8f, 0.6f, 0.4f);
            metallic = 0.0f;
            roughness = 0.8f;
            specularStrength = 0.2f;
            break;
        case MATERIAL_METAL:
            if (!useTexture)
                baseColor = glm::vec3(0.7f, 0.7f, 0.8f);
            metallic = 1.0f;
            roughness = 0.2f;
            specularStrength = 0.9f;
            break;
        case MATERIAL_PLASTIC:
            if (!useTexture)
                baseColor = glm::vec3(0.2f, 0.2f, 0.2f);
            metallic = 0.0f;
            roughness = 0.4f;
            specularStrength = 0.6f;
            break;
        case MATERIAL_FABRIC:
            if (!useTexture)
                baseColor = glm::vec3(0.4f, 0.4f, 0.6f);
            metallic = 0.0f;
            roughness = 0.9f;
            specularStrength = 0.1f;
            break;
        }
    }

    void Delete()
    {
        if (diffuseTexture)
        {
            diffuseTexture->Delete();
            delete diffuseTexture;
            diffuseTexture = nullptr;
        }
    }
};

#endif