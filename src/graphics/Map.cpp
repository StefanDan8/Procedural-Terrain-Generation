#include "Map.hpp"
#include "../lodepng/lodepng.h"

void Map::exportToPNG(const std::string &filename) const {
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

void Map::exportToPPM(const std::string &filename) const {
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
