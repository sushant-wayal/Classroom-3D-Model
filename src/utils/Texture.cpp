#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Texture::Texture(const char *imagePath, GLenum texType, GLint wrap, GLint filter)
{
    type = texType;
    loadTexture(imagePath, texType, wrap, filter);
}

void Texture::loadTexture(const char *imagePath, GLenum texType, GLint wrap, GLint filter)
{
    glGenTextures(1, &ID);
    glBindTexture(texType, ID);

    glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, filter);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(texType, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(texType);

        std::cout << "Texture loaded successfully: " << imagePath << " (" << width << "x" << height << ", " << nrChannels << " channels)" << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture: " << imagePath << std::endl;
      
        unsigned char defaultData[] = {255, 255, 255, 255};
        glTexImage2D(texType, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, defaultData);
        width = height = 1;
        nrChannels = 4;
    }

    stbi_image_free(data);
    glBindTexture(texType, 0);
}

void Texture::texUnit(unsigned int shader, const char *uniform, GLuint unit)
{
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, uniform), unit);
}

void Texture::Bind()
{
    glBindTexture(type, ID);
}

void Texture::Unbind()
{
    glBindTexture(type, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}