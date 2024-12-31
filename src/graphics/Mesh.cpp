#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
   Mesh::vertices = vertices;
   Mesh::indices = indices;

   myVAO.Bind();
   VBO VBO(vertices);
   EBO EBO(indices);

   myVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*) 0); // coordinates
   myVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*) (3 * sizeof(float))); // color

   myVAO.Unbind();
   VBO.Unbind();
   EBO.Unbind();
}

Mesh::Mesh(std::vector<std::vector<double>>& matrix) {
   unsigned numX = matrix.size() - 1;
   unsigned numY = matrix[0].size() - 1;
   unsigned numVertices = (numX + 1) * (numY + 1);
   unsigned numFaces = numX * numY;
   // Mesh::vertices.resize(numVertices);
   // Mesh::indices.resize(numFaces * 4);
   std::vector<Vertex> _vertices(numVertices);
   std::vector<GLuint> _indices(numFaces * 6);
   float invNumX = 1.0f / numX;
   float invNumY = 1.0f / numY;

   for (unsigned j = 0; j <= numY; ++j) {
      for (unsigned i = 0; i <= numX; ++i) {
         _vertices[j * (numX + 1) + i] = Vertex{glm::vec3(i * invNumX - 0.5f, (float) matrix[i][j], j * invNumY - 0.5f), glm::vec3(0.3f, 0.70f, 0.44f)};
      }
   }
   for (unsigned j = 0, k = 0; j < numY; ++j) {
      for (unsigned i = 0; i < numX; ++i) {
         _indices[k] = j * (numX + 1) + i;
         _indices[k + 1] = j * (numX + 1) + i + 1;
         _indices[k + 2] = (j + 1) * (numX + 1) + i + 1;
         _indices[k + 3] = (j + 1) * (numX + 1) + i + 1;
         _indices[k + 4] = (j + 1) * (numX + 1) + i;
         _indices[k + 5] = j * (numX + 1) + i;
         k += 6;
      }
   }

   Mesh::vertices = _vertices;
   Mesh::indices = _indices;

   myVAO.Bind();
   VBO VBO(vertices);
   EBO EBO(indices);

   myVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*) 0); // coordinates
   myVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*) (3 * sizeof(float))); // color

   myVAO.Unbind();
   VBO.Unbind();
   EBO.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera) {
   shader.Activate();
   myVAO.Bind();
   glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
   camera.Matrix(shader, "camMatrix");
   glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void PrintMesh(const Mesh& mesh) {
   std::cout << "{\n";
   for (const auto& vertex : mesh.vertices) {
      std::cout << "   Vertex{glm::vec3("
                << std::fixed << std::setprecision(3) // Set decimal precision to 1
                << vertex.position.x << "f, "
                << vertex.position.y << "f, "
                << vertex.position.z << "f), glm::vec3("
                << vertex.color.x << "f, "
                << vertex.color.y << "f, "
                << vertex.color.z << "f)},\n";
   }
   std::cout << "};\n\n";

   std::cout << "   {\n      ";
   for (size_t i = 0; i < mesh.indices.size(); ++i) {
      std::cout << mesh.indices[i];
      if (i < mesh.indices.size() - 1) {
         std::cout << ", ";
      }
      if ((i + 1) % 3 == 0 && i < mesh.indices.size() - 1) {
         std::cout << "\n      ";
      }
   }
   std::cout << "\n   };\n";
}