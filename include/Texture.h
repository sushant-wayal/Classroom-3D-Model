#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

class Texture
{
public:
    GLuint ID;
    GLenum type;

    // Placeholder constructor for now
    Texture() : ID(0), type(GL_TEXTURE_2D) {}

    // Assigns a texture unit to a texture
    void texUnit(unsigned int shader, const char *uniform, GLuint unit) {}
    // Binds a texture
    void Bind() {}
    // Unbinds a texture
    void Unbind() {}
    // Deletes a texture
    void Delete() {}
};

#endif