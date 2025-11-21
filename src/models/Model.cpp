#include "models/Model.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

void Mesh::setupMesh()
{
    meshVAO.Bind();

    meshVBO = new VBO((GLfloat *)vertices.data(), vertices.size() * sizeof(Vertex));
    meshEBO = new EBO(indices.data(), indices.size() * sizeof(unsigned int));

    // Position attribute (location = 0)
    meshVAO.LinkVBOAttrib(*meshVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    // Normal attribute (location = 1)
    meshVAO.LinkVBOAttrib(*meshVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(offsetof(Vertex, Normal)));
    // Texture coordinate attribute (location = 2)
    meshVAO.LinkVBOAttrib(*meshVBO, 2, 2, GL_FLOAT, sizeof(Vertex), (void *)(offsetof(Vertex, TexCoords)));

    meshVAO.Unbind();
    meshVBO->Unbind();
    meshEBO->Unbind();
}

void Mesh::Draw(Shader &shader)
{
    // Bind material properties
    if (material)
    {
        if (material->diffuseMap != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            material->diffuseMap->Bind();
            material->diffuseMap->texUnit(shader.ID, "tex0", 0);
            glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), 1);
        }
        else
        {
            // No texture, use material diffuse color
            glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), 0);
            glUniform3fv(glGetUniformLocation(shader.ID, "materialDiffuse"), 1, glm::value_ptr(material->diffuse));
        }
    }
    else
    {
        glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), 0);
    }

    meshVAO.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    meshVAO.Unbind();

    if (material && material->diffuseMap != nullptr)
    {
        material->diffuseMap->Unbind();
    }
}

void Mesh::Delete()
{
    meshVAO.Delete();
    if (meshVBO)
    {
        meshVBO->Delete();
        delete meshVBO;
        meshVBO = nullptr;
    }
    if (meshEBO)
    {
        meshEBO->Delete();
        delete meshEBO;
        meshEBO = nullptr;
    }
}

Model::Model(const char *objFile)
{
    loadOBJ(objFile);
}

Model::Model(const char *objFile, const char *texturePath)
{
    // Load OBJ with materials, then override with single texture
    loadOBJ(objFile);

    // Apply single texture to all meshes (legacy support)
    for (auto &mesh : meshes)
    {
        if (mesh.material && mesh.material->diffuseMap == nullptr)
        {
            mesh.material->diffuseMap = new Texture(texturePath);
        }
    }
}

void Model::loadMTL(const std::string &mtlFile, const std::string &basePath)
{
    std::ifstream file(mtlFile);
    if (!file.is_open())
    {
        std::cerr << "Failed to open MTL file: " << mtlFile << std::endl;
        return;
    }

    Material *currentMaterial = nullptr;
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "newmtl")
        {
            std::string materialName;
            iss >> materialName;

            currentMaterial = new Material();
            currentMaterial->name = materialName;
            materials[materialName] = currentMaterial;

            std::cout << "Loading material: " << materialName << std::endl;
        }
        else if (currentMaterial != nullptr)
        {
            if (prefix == "Ka")
            {
                iss >> currentMaterial->ambient.x >> currentMaterial->ambient.y >> currentMaterial->ambient.z;
            }
            else if (prefix == "Kd")
            {
                iss >> currentMaterial->diffuse.x >> currentMaterial->diffuse.y >> currentMaterial->diffuse.z;
            }
            else if (prefix == "Ks")
            {
                iss >> currentMaterial->specular.x >> currentMaterial->specular.y >> currentMaterial->specular.z;
            }
            else if (prefix == "Ns")
            {
                iss >> currentMaterial->shininess;
            }
            else if (prefix == "map_Kd")
            {
                std::string texturePath;
                iss >> texturePath;

                // Check if path is absolute or relative
                if (texturePath[0] == '/')
                {
                    // Absolute path
                    currentMaterial->diffuseMap = new Texture(texturePath.c_str());
                    std::cout << "  -> Loaded texture (absolute): " << texturePath << std::endl;
                }
                else
                {
                    // Relative path
                    std::string fullPath = basePath + texturePath;
                    currentMaterial->diffuseMap = new Texture(fullPath.c_str());
                    std::cout << "  -> Loaded texture (relative): " << fullPath << std::endl;
                }
            }
        }
    }

    file.close();
    std::cout << "Loaded " << materials.size() << " materials from MTL file" << std::endl;
}

void Model::loadOBJ(const char *objFile)
{
    std::ifstream file(objFile);
    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << objFile << std::endl;
        return;
    }

    // Extract base path from obj file path
    std::string objPath(objFile);
    std::string basePath = objPath.substr(0, objPath.find_last_of("/\\") + 1);

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    Mesh currentMesh;
    Material *currentMaterial = nullptr;
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
        else if (prefix == "mtllib")
        {
            // MTL library reference
            std::string mtlFileName;
            iss >> mtlFileName;
            std::string mtlPath = basePath + mtlFileName;
            loadMTL(mtlPath, basePath);
        }
        else if (prefix == "usemtl")
        {
            // Material switch - save current mesh and start new one
            std::string materialName;
            iss >> materialName;

            // Save previous mesh if it has data
            if (!currentMesh.vertices.empty())
            {
                currentMesh.material = currentMaterial;
                currentMesh.setupMesh();
                meshes.push_back(currentMesh);
            }

            // Start new mesh with new material
            currentMesh = Mesh();
            vertexMap.clear();

            if (materials.find(materialName) != materials.end())
            {
                currentMaterial = materials[materialName];
            }
            else
            {
                std::cout << "Warning: Material '" << materialName << "' not found!" << std::endl;
                currentMaterial = nullptr;
            }
        }
        else if (prefix == "f")
        {
            // Read all vertices in the face
            std::vector<std::string> faceVertices;
            std::string vertexStr;
            while (iss >> vertexStr)
            {
                faceVertices.push_back(vertexStr);
            }

            // Handle triangles and quads
            if (faceVertices.size() == 3)
            {
                // Triangle
                for (const std::string &vertexStr : faceVertices)
                {
                    processVertex(vertexStr, temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                }
            }
            else if (faceVertices.size() == 4)
            {
                // Quad - triangulate
                processVertex(faceVertices[0], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                processVertex(faceVertices[1], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                processVertex(faceVertices[2], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);

                processVertex(faceVertices[0], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                processVertex(faceVertices[2], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                processVertex(faceVertices[3], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
            }
            else if (faceVertices.size() > 4)
            {
                // N-gon - fan triangulation
                for (size_t i = 1; i < faceVertices.size() - 1; i++)
                {
                    processVertex(faceVertices[0], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                    processVertex(faceVertices[i], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                    processVertex(faceVertices[i + 1], temp_vertices, temp_uvs, temp_normals, vertexMap, currentMesh);
                }
            }
        }
    }

    // Save final mesh
    if (!currentMesh.vertices.empty())
    {
        currentMesh.material = currentMaterial;
        currentMesh.setupMesh();
        meshes.push_back(currentMesh);
    }

    file.close();
}

void Model::processVertex(const std::string &vertexStr, const std::vector<glm::vec3> &temp_vertices,
                          const std::vector<glm::vec2> &temp_uvs, const std::vector<glm::vec3> &temp_normals,
                          std::map<std::string, unsigned int> &vertexMap, Mesh &currentMesh)
{
    // Check if we've already processed this exact vertex combination
    auto it = vertexMap.find(vertexStr);
    if (it != vertexMap.end())
    {
        // Reuse existing vertex
        currentMesh.indices.push_back(it->second);
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
            vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

        currentMesh.vertices.push_back(vertex);
        unsigned int newIndex = currentMesh.vertices.size() - 1;
        vertexMap[vertexStr] = newIndex;
        currentMesh.indices.push_back(newIndex);
    }
}

void Model::Draw(Shader &shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
    shader.Activate();

    // Pass matrices to shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Draw all submeshes with their respective materials
    for (auto &mesh : meshes)
    {
        mesh.Draw(shader);
    }
}

void Model::Delete()
{
    // Delete all meshes
    for (auto &mesh : meshes)
    {
        mesh.Delete();
    }
    meshes.clear();

    // Delete all materials and their textures
    for (auto &pair : materials)
    {
        if (pair.second->diffuseMap != nullptr)
        {
            pair.second->diffuseMap->Delete();
            delete pair.second->diffuseMap;
        }
        delete pair.second;
    }
    materials.clear();
}