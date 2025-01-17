#ifndef MAP_CLASS_HPP
#define MAP_CLASS_HPP

#include "Mesh.hpp"

/**
 * 2D implementation of the Mesh class.
 * @author PK
 */
class Map: public Mesh {
   // Size of the map, so it doesn't need to be recalculated later.
   unsigned long sizeX, sizeY;

public:
   /**
    * Constructor for the Map class.
    * @param matrix Normalised matrix to be used as the map
    */
   explicit Map(std::vector<std::vector<double>>& matrix): Mesh(matrix) { sizeX = matrix.size(); sizeY = matrix[0].size(); }

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

#endif