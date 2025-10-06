#include "models/Model.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

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
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;

            // Parse face indices (format: v/vt/vn)
            for (const std::string &vertexStr : {vertex1, vertex2, vertex3})
            {
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
                if (uvIndex < temp_uvs.size())
                    vertex.TexCoords = temp_uvs[uvIndex];
                if (normalIndex < temp_normals.size())
                    vertex.Normal = temp_normals[normalIndex];

                vertices.push_back(vertex);
                indices.push_back(vertices.size() - 1);
            }
        }
    }

    file.close();
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