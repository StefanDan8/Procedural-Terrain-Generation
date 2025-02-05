#include "Mesh.hpp"

#include <filesystem>

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices) {
   Mesh::vertices = vertices;
   Mesh::indices = indices;

   // Calculate size of the mesh by counting the vertices on the x axis, assuming the mesh is a rectangle and the vertices are ordered in a grid.
   sizeX = 0;
   float loopBackValue = vertices[0].position.z;

   for (Vertex vtx : vertices) {
      if (vtx.position.z == loopBackValue)
         sizeX++;
      else
         break;
   }

   sizeY = vertices.size() / sizeX;

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

Mesh::Mesh(const std::vector<std::vector<double>>& matrix) {
   // Saving the size of the matrix is easy here.
   sizeX = matrix.size();
   sizeY = matrix[0].size();

   unsigned numX = matrix.size() - 1;
   unsigned numY = matrix[0].size() - 1;
   unsigned numVertices = (numX + 1) * (numY + 1);
   unsigned numFaces = numX * numY;
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

void ExportToObj(const Mesh& mesh, const std::string& filename) {
   std::ofstream file(filename);

   if (!file.is_open()) {
      std::cerr << "Failed to open file: " << filename << std::endl;
      return;
   }

   // Write vertex positions
   for (const auto& vertex : mesh.vertices) {
      file << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
   }
   // Write normals
   for (const auto& vertex : mesh.vertices) {
      file << "vn " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << "\n";
   }

   // Write texture coordinates
   for (const auto& vertex : mesh.vertices) {
      file << "vt " << vertex.texUV.x << " " << vertex.texUV.y << "\n";
   }

   // Write faces
   for (size_t i = 0; i < mesh.indices.size(); i += 3) {
      file << "f "
           << (mesh.indices[i] + 1) << "/" << (mesh.indices[i] + 1) << "/" << (mesh.indices[i] + 1) << " "
           << (mesh.indices[i + 1] + 1) << "/" << (mesh.indices[i + 1] + 1) << "/" << (mesh.indices[i + 1] + 1) << " "
           << (mesh.indices[i + 2] + 1) << "/" << (mesh.indices[i + 2] + 1) << "/" << (mesh.indices[i + 2] + 1) << "\n";
   }

   file.close();
   std::cout << "Mesh exported to " << filename << " successfully." << std::endl;
}

void Mesh::exportToPNG(const std::string& filename) const {
   std::vector<unsigned char> image; // Create image vector to store "pixels"
   image.resize(sizeX * sizeY * 4);

   for (unsigned x = 0; x < sizeX; x++) {
      for (unsigned y = 0; y < sizeY; y++) {
         unsigned index = 4 * sizeX * y + 4 * x;
         int value = (255 * vertices[y * sizeX + x].position.y);
         value = (value < 0) ? 0 : value;
         image[index] = (unsigned char) value;
         image[index + 1] = (unsigned char) value;
         image[index + 2] = (unsigned char) value;
         image[index + 3] = 255;
      }
   }

   std::vector<unsigned char> png;

   // Write image to file
   unsigned error = lodepng::encode(png, image, sizeX, sizeY);
   if (!error) lodepng::save_file(png, filename);

   //if there's an error, display it
   if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

void Mesh::exportToPPM(const std::string& filename) const {
   // Open stream to file
   std::ofstream file(filename);
   if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
   }

   // Write file header
   file << "P3 " << sizeX << " " << sizeY << " 255\n";

   // Write file content
   for (unsigned x = 0; x < sizeX; x++) {
      for (unsigned y = 0; y < sizeY; y++) {
         int value = (255 * vertices[y * sizeX + x].position.y);
         value = (value < 0) ? 0 : value;
         file << value << " " << value << " " << value << " ";
      }
   }

   file.close();
}
