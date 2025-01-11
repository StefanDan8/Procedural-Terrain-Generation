#ifndef VAO_CLASS_HPP
#define VAO_CLASS_HPP

#include "VBO.hpp"
#include <glad/glad.h>

/// @brief Class for handling openGL Vertex Array Objects
/// A VAO encapsulates all the state needed to supply vertex data i.e. what data is stored in VBO and how it is organized
/// @cite Implementation following Victor Gordan's openGL tutorial
/// https://github.com/VictorGordan/opengl-tutorials
class VAO {
   public:
   GLuint ID;
   VAO();

   void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

   void Bind();
   void Unbind();
   void Delete();
};

#endif