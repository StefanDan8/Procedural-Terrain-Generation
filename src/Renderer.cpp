#include "Renderer.hpp"

#include <cmath>
#include <fstream>
#include <stdexcept>
#include "lodepng/lodepng.h"

namespace render {
void create_ppm(const std::vector<std::vector<int>>& noise, const std::string& filename) {
   // Check vector size is correct size
   int width = noise.size();
   int height = noise[0].size();
   if (width < 1 || height < 1) {
      throw std::invalid_argument("Width and height must be greater than 0");
   }

   // Check filename provided ends in .ppm
   int len = filename.length();
   if (filename.substr(len - 4) != ".ppm") {
      throw std::invalid_argument("Filename provided must end in .ppm");
   }
   std::string fullPath = "../output/" + filename;

   // Open stream to file
   std::ofstream file(fullPath);
   if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
   }

   // Write file header
   file << "P3 " << width << " " << height << " 255\n";

   // Write file content
   for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
         int value = noise[x][y]; // (int) std::round((noise[index] + 1.0) * 127.5); // Scale noise to [0, 255]
         file << value << " " << value << " " << value << " ";
      }
   }

   file.close();
}

void create_png(const std::vector<std::vector<int>>& noise, const std::string& filename) {
   // Check filename provided ends in .png
   int len = filename.length();
   if (filename.substr(len - 4) != ".png") {
      throw std::invalid_argument("Filename provided must end in .png");
   }
   std::string fullPath = "../output/" + filename;

   std::vector<unsigned char> image; // Create image vector to store "pixels"
   unsigned width = noise.size(), height = noise[0].size();
   image.resize(width * height * 4); // Resize image vector to store RGBA values

   for (unsigned x = 0; x < width; x++) {
      for (unsigned y = 0; y < height; y++) {
         image[4 * width * y + 4 * x + 0] = noise[x][y];
         image[4 * width * y + 4 * x + 1] = noise[x][y];
         image[4 * width * y + 4 * x + 2] = noise[x][y];
         image[4 * width * y + 4 * x + 3] = 255;
      }
   }

   std::vector<unsigned char> png;

   // Write image to file
   unsigned error = lodepng::encode(png, image, width, height);
   if (!error) lodepng::save_file(png, fullPath);

   //if there's an error, display it
   if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

std::vector<std::vector<int>> normalizeMatrix(const std::vector<std::vector<double>>& matrix) {
   size_t rows = matrix.size();
   size_t cols = matrix[0].size();

   // Find the minimum and maximum values in the matrix
   double minVal = matrix[0][0];
   double maxVal = matrix[0][0];
   for (const auto& row : matrix) {
      for (double value : row) {
         minVal = std::min(minVal, value);
         maxVal = std::max(maxVal, value);
      }
   }

   // Normalize the matrix to [0, 255]
   std::vector<std::vector<int>> normalized(rows, std::vector<int>(cols));
   for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < cols; ++j) {
         normalized[i][j] = static_cast<int>(
            255 * (matrix[i][j] - minVal) / (maxVal - minVal));
      }
   }

   return normalized;
}

void writeMatrixToFile(const std::vector<std::vector<int>>& matrix, const std::string& filename) {
   std::string fullPath = "../output/" + filename;

   std::ofstream file(fullPath);
   if (!file.is_open()) {
      std::cerr << "Error: Could not open file " << fullPath << std::endl;
      return;
   }

   for (const auto& row : matrix) {
      for (int value : row) {
         file << std::setw(4) << value;
      }
      file << '\n';
   }

   file.close();
   std::cout << "Matrix written to " << fullPath << std::endl;
}

}