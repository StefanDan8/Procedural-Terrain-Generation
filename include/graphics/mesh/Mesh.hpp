#ifndef MESH_CLASS_HPP
#define MESH_CLASS_HPP

#include "lodepng.h"
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

   // Size of the mesh, so it doesn't need to be recalculated later during 2D saves.
   unsigned long sizeX, sizeY;

   VAO myVAO;

   Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
   Mesh(const std::vector<std::vector<double>>& matrix);

   /**
    * Shows the mesh in the rendering area.
    * @param shader Shader to be used for rendering
    * @param camera The camera from which the mesh is being viewed from
    * @author SD
    */
   void Draw(Shader& shader, Camera& camera);

   /**
    * Exports the current map to a PNG file.
    * @param filename Name (and location) of the file to be saved
    * @author PK
    */
   void exportToPNG(const std::string& filename) const;

   /**
    * Exports the current map to a NetBPM (PPM) file.
    * @param filename Name (and location) of the file to be saved
    * @author PK
    */
   void exportToPPM(const std::string& filename) const;
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