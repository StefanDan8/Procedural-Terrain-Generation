#ifndef MESH_CLASS_HPP
#define MESH_CLASS_HPP

#include "Camera.hpp"
#include "EBO.hpp"
#include "VAO.hpp"
#include <iomanip>
#include <string>
#include <vector>

class Mesh {
   public:
   std::vector<Vertex> vertices;
   std::vector<GLuint> indices;

   VAO myVAO;

   Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
   Mesh(std::vector<std::vector<double>>& matrix);
   void Draw(Shader& shader, Camera& camera);
};

void PrintMesh(const Mesh& mesh);

#endif