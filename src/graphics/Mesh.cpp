#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
   Mesh::vertices = vertices;
   Mesh::indices = indices;

   myVAO.Bind();
   VBO VBO(vertices);
   EBO EBO(indices);

   myVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*) 0); // coordinates
   myVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*) (3 * sizeof(float))); // normal
   myVAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*) (6 * sizeof(float))); // color
   myVAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*) (9 * sizeof(float))); // texture coordinates

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
         glm::vec3 coordinates = glm::vec3(i * invNumX - 0.5f, matrix[i][j], j * invNumY - 0.5f);
         glm::vec3 normal = glm::vec3(0.0f); //TODO: compute normal
         glm::vec2 texCoordinates = glm::vec2(i * invNumX, j * invNumY);

         _vertices[j * (numX + 1) + i] = Vertex{coordinates, normal, glm::vec3(0.3f, 0.70f, 0.44f), texCoordinates};
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

   // Compute normals per face
   for (size_t i = 0; i < indices.size(); i += 3) {
      GLuint i0 = indices[i];
      GLuint i1 = indices[i + 1];
      GLuint i2 = indices[i + 2];

      glm::vec3 v0 = vertices[i0].position;
      glm::vec3 v1 = vertices[i1].position;
      glm::vec3 v2 = vertices[i2].position;

      // Compute the face normal
      glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

      // Add the face normal to each vertex normal
      vertices[i0].normal += normal;
      vertices[i1].normal += normal;
      vertices[i2].normal += normal;
   }

   // Normalize the vertex normals
   for (auto& vertex : vertices) {
      vertex.normal = glm::normalize(-vertex.normal);
   }

   myVAO.Bind();
   VBO VBO(vertices);
   EBO EBO(indices);

   myVAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*) 0); // coordinates
   myVAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*) (3 * sizeof(float))); // normal
   myVAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*) (6 * sizeof(float))); // color
   myVAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*) (9 * sizeof(float))); // texture coordinates

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

void ComputeNormals(Mesh& mesh) {
   // Initialize normals to zero
   for (auto& vertex : mesh.vertices) {
      vertex.normal = glm::vec3(0.0f);
   }

   // Compute normals per face
   for (size_t i = 0; i < mesh.indices.size(); i += 3) {
      GLuint i0 = mesh.indices[i];
      GLuint i1 = mesh.indices[i + 1];
      GLuint i2 = mesh.indices[i + 2];

      glm::vec3 v0 = mesh.vertices[i0].position;
      glm::vec3 v1 = mesh.vertices[i1].position;
      glm::vec3 v2 = mesh.vertices[i2].position;

      // Compute the face normal
      glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

      // Add the face normal to each vertex normal
      mesh.vertices[i0].normal += normal;
      mesh.vertices[i1].normal += normal;
      mesh.vertices[i2].normal += normal;
   }

   // Normalize the vertex normals
   for (auto& vertex : mesh.vertices) {
      vertex.normal = glm::normalize(-vertex.normal);
   }
}

void PrintMesh(const Mesh& mesh) {
   std::cout << "{\n";
   for (const auto& vertex : mesh.vertices) {
      std::cout << "   Vertex{glm::vec3("
                << std::fixed << std::setprecision(3) // Set decimal precision to 1
                << vertex.position.x << "f, "
                << vertex.position.y << "f, "
                << vertex.position.z << "f), glm::vec3("
                << vertex.normal.x << "f, "
                << vertex.normal.y << "f, "
                << vertex.normal.z << "f)},\n";
   }
   std::cout << "};\n\n";

   // std::cout << "   {\n      ";
   // for (size_t i = 0; i < mesh.indices.size(); ++i) {
   //    std::cout << mesh.indices[i];
   //    if (i < mesh.indices.size() - 1) {
   //       std::cout << ", ";
   //    }
   //    if ((i + 1) % 3 == 0 && i < mesh.indices.size() - 1) {
   //       std::cout << "\n      ";
   //    }
   // }
   // std::cout << "\n   };\n";
}