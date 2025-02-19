#ifndef EBO_CLASS_HPP
#define EBO_CLASS_HPP

#include <vector>
#include <glad/glad.h>

/// @brief Class for handling openGL Element Buffer Objects
/// An EBO stores indices that define how vertices are connected to form primitives (e.g. triangles)
/// Advantage: Instead of duplicating vertices, they are referenced by index, reducing memory usage
/// @cite Implementation following Victor Gordan's openGL tutorial
/// https://github.com/VictorGordan/opengl-tutorials
class EBO {
   public:
   GLuint ID;
   EBO(std::vector<GLuint>& indices);

   void Bind();
   void Unbind();
   void Delete();
};
#endif