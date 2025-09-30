#ifndef VBO_H
#define VBO_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class VBO {
    public:
        GLuint ID;

        VBO(GLfloat* vertices, GLsizeiptr size);

        void Bind();

        void Unbind();

        void Delete();
};

#endif