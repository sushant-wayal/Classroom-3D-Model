#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <iostream>

class Texture
{
public:
    GLuint ID;
    GLenum type;
    int width, height, nrChannels;

    // Constructor that loads an image file
    Texture(const char *imagePath, GLenum texType = GL_TEXTURE_2D, GLint wrap = GL_REPEAT, GLint filter = GL_LINEAR);

    // Default constructor for placeholder
    Texture() : ID(0), type(GL_TEXTURE_2D), width(0), height(0), nrChannels(0) {}

    // Assigns a texture unit to a texture
    void texUnit(unsigned int shader, const char *uniform, GLuint unit);
    // Binds a texture
    void Bind();
    // Unbinds a texture
    void Unbind();
    // Deletes a texture
    void Delete();

private:
    void loadTexture(const char *imagePath, GLenum texType, GLint wrap, GLint filter);
};

#endif