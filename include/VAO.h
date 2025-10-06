#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "VBO.h"

class VAO {
    public:
        GLuint ID;

        VAO();

        void LinkVBOAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

        void Bind();

        void Unbind();

        void Delete();
};

#endif