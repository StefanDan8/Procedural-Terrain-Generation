#ifndef MAP_CLASS_HPP
#define MAP_CLASS_HPP

#include "Mesh.hpp"

class Map: public Mesh {
   unsigned long sizeX, sizeY;

public:
   explicit Map(std::vector<std::vector<double>>& matrix): Mesh(matrix) { sizeX = matrix.size(); sizeY = matrix[0].size(); }

   void exportToPNG(const std::string& filename) const;
   void exportToPPM(const std::string& filename) const;
};

#endif