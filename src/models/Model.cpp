#include "models/Model.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <map>

// Comparison struct for glm::vec3 to use in std::map
struct CompareVec3
{
    bool operator()(const glm::vec3 &a, const glm::vec3 &b) const
    {
        const float epsilon = 0.0001f;
        if (std::abs(a.x - b.x) > epsilon)
            return a.x < b.x;
        if (std::abs(a.y - b.y) > epsilon)
            return a.y < b.y;
        return a.z < b.z;
    }
};

Model::Model(const char *objFile)
{
    loadOBJ(objFile);
    setupModel();
}

void Model::loadOBJ(const char *objFile)
{
    std::ifstream file(objFile);
    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << objFile << std::endl;
        return;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    // Use a map to avoid duplicate vertices and enable proper normal interpolation
    std::map<std::string, unsigned int> vertexMap;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt")
        {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn")
        {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f")
        {
            // Read all vertices in the face (could be triangle or quad)
            std::vector<std::string> faceVertices;
            std::string vertexStr;
            while (iss >> vertexStr)
            {
                faceVertices.push_back(vertexStr);
            }

            // Handle both triangles (3 vertices) and quads (4 vertices)
            if (faceVertices.size() == 3)
            {
                // Triangle - process normally
                for (const std::string &vertexStr : faceVertices)
                {
                    processVertex(vertexStr, temp_vertices, temp_uvs, temp_normals, vertexMap);
                }
            }
            else if (faceVertices.size() == 4)
            {
                // Quad - triangulate into 2 triangles
                // Triangle 1: vertices 0, 1, 2
                processVertex(faceVertices[0], temp_vertices, temp_uvs, temp_normals, vertexMap);
                processVertex(faceVertices[1], temp_vertices, temp_uvs, temp_normals, vertexMap);
                processVertex(faceVertices[2], temp_vertices, temp_uvs, temp_normals, vertexMap);

                // Triangle 2: vertices 0, 2, 3
                processVertex(faceVertices[0], temp_vertices, temp_uvs, temp_normals, vertexMap);
                processVertex(faceVertices[2], temp_vertices, temp_uvs, temp_normals, vertexMap);
                processVertex(faceVertices[3], temp_vertices, temp_uvs, temp_normals, vertexMap);
            }
            else if (faceVertices.size() > 4)
            {
                // N-gon - triangulate using fan triangulation
                for (size_t i = 1; i < faceVertices.size() - 1; i++)
                {
                    processVertex(faceVertices[0], temp_vertices, temp_uvs, temp_normals, vertexMap);
                    processVertex(faceVertices[i], temp_vertices, temp_uvs, temp_normals, vertexMap);
                    processVertex(faceVertices[i + 1], temp_vertices, temp_uvs, temp_normals, vertexMap);
                }
            }
        }
    }

    file.close();

    // Generate smooth normals only if needed
    bool needsNormalGeneration = temp_normals.empty();
    if (!needsNormalGeneration)
    {
        // Check if any normals are zero (indicating missing normals)
        for (const auto &vertex : vertices)
        {
            if (glm::length(vertex.Normal) < 0.1f)
            {
                needsNormalGeneration = true;
                break;
            }
        }
    }

    if (needsNormalGeneration)
    {
        std::cout << "Generating smooth normals for model: " << objFile << std::endl;
        generateSmoothNormals();
    }

    std::cout << "Loaded model: " << objFile << " with " << vertices.size()
              << " vertices and " << indices.size() / 3 << " faces" << std::endl;
}

void Model::generateSmoothNormals()
{
    // Reset all normals to zero
    for (auto &vertex : vertices)
    {
        vertex.Normal = glm::vec3(0.0f);
    }

    // Calculate face normals and add them to vertex normals
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size())
            continue;

        glm::vec3 v0 = vertices[i0].Position;
        glm::vec3 v1 = vertices[i1].Position;
        glm::vec3 v2 = vertices[i2].Position;

        // Calculate face normal
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        // Add face normal to each vertex of the triangle
        vertices[i0].Normal += faceNormal;
        vertices[i1].Normal += faceNormal;
        vertices[i2].Normal += faceNormal;
    }

    // Normalize all vertex normals
    for (auto &vertex : vertices)
    {
        if (glm::length(vertex.Normal) > 0.0f)
        {
            vertex.Normal = glm::normalize(vertex.Normal);
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default upward normal
        }
    }
}

void Model::setupModel()
{
    modelVAO.Bind();

    modelVBO = new VBO((GLfloat *)vertices.data(), vertices.size() * sizeof(Vertex));
    modelEBO = new EBO(indices.data(), indices.size() * sizeof(unsigned int));

    // Position attribute
    modelVAO.LinkVBOAttrib(*modelVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    // Normal attribute
    modelVAO.LinkVBOAttrib(*modelVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
    // Texture coordinate attribute
    modelVAO.LinkVBOAttrib(*modelVBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void *)(offsetof(Vertex, TexCoords)));

    modelVAO.Unbind();
    modelVBO->Unbind();
    modelEBO->Unbind();
}

void Model::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.Activate();

    // Pass matrices to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    modelVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    modelVAO.Unbind();
}

void Model::Delete()
{
    modelVAO.Delete();
    if (modelVBO)
    {
        modelVBO->Delete();
        delete modelVBO;
    }
    if (modelEBO)
    {
        modelEBO->Delete();
        delete modelEBO;
    }
}

void Model::processVertex(const std::string &vertexStr, const std::vector<glm::vec3> &temp_vertices, const std::vector<glm::vec2> &temp_uvs, const std::vector<glm::vec3> &temp_normals, std::map<std::string, unsigned int> &vertexMap)
{
    // Check if we've already processed this exact vertex combination
    auto it = vertexMap.find(vertexStr);
    if (it != vertexMap.end())
    {
        // Reuse existing vertex
        indices.push_back(it->second);
    }
    else
    {
        // Create new vertex
        std::string mutableVertexStr = vertexStr;
        std::replace(mutableVertexStr.begin(), mutableVertexStr.end(), '/', ' ');
        std::istringstream vertexStream(mutableVertexStr);

        unsigned int vertexIndex, uvIndex, normalIndex;
        vertexStream >> vertexIndex >> uvIndex >> normalIndex;

        // OBJ indices are 1-based, convert to 0-based
        vertexIndex--;
        uvIndex--;
        normalIndex--;

        Vertex vertex;
        if (vertexIndex < temp_vertices.size())
            vertex.Position = temp_vertices[vertexIndex];
        else
            vertex.Position = glm::vec3(0.0f);

        if (uvIndex < temp_uvs.size())
            vertex.TexCoords = temp_uvs[uvIndex];
        else
            vertex.TexCoords = glm::vec2(0.0f);

        if (normalIndex < temp_normals.size())
            vertex.Normal = glm::normalize(temp_normals[normalIndex]);
        else
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default normal

        vertices.push_back(vertex);
        unsigned int newIndex = vertices.size() - 1;
        vertexMap[vertexStr] = newIndex;
        indices.push_back(newIndex);
    }
}