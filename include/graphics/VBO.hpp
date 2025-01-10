#ifndef VBO_CLASS_HPP
#define VBO_CLASS_HPP

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

struct Vertex {
   glm::vec3 position;
   glm::vec3 normal;
   glm::vec3 color;
   /// @note texture coordinates (not used yet)
   glm::vec2 texUV;
};

/// @brief Class for handling openGL Vertex Buffer Objects
/// A VBO stores vertex data in the GPU's memory for rendering
/// In this implementation, this vertex data is organized in the `Vertex` struct
/// @cite Implementation following Victor Gordan's openGL tutorial
/// https://github.com/VictorGordan/opengl-tutorials
class VBO {
   public:
   GLuint ID;
   VBO(std::vector<Vertex>& vertices);
   void Bind();
   void Unbind();
   void Delete();
};
#endif