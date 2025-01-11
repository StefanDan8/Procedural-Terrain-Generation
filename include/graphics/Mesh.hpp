#ifndef MESH_CLASS_HPP
#define MESH_CLASS_HPP

#include "Camera.hpp"
#include "EBO.hpp"
#include "VAO.hpp"
#include <iomanip>
#include <string>
#include <vector>

/**
 * Class for storing and managing the vertices and indices of a mesh.
 * @author SD
 */
class Mesh {
   public:
   std::vector<Vertex> vertices;
   std::vector<GLuint> indices;

   VAO myVAO;

   Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
   Mesh(std::vector<std::vector<double>>& matrix);

   /**
    * Shows the mesh in the rendering area.
    * @param shader Shader to be used for rendering
    * @param camera The camera from which the mesh is being viewed from
    * @author SD
    */
   void Draw(Shader& shader, Camera& camera);
};

// void ComputeNormals(Mesh& mesh);

/**
 * Exports the mesh to an .obj file.
 * @param mesh The mesh to be exported
 * @param filename Name of the file to be saved to
 * @author SD
 */
void ExportToObj(const Mesh& mesh, const std::string& filename);

#endif