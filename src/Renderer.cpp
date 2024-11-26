#include "Renderer.hpp"

#include <cmath>
#include <fstream>
#include <stdexcept>

// -----------------------------------------------------------------------------
/* The goal here is to render the results from the Perlin noise generator.
   To do so, we will first write to a NetPBM file, and then use something to convert it into a .png file.
   For now, we will only render a 2D image.
 */
namespace render {

void ppm(const std::vector<double>& noise, const unsigned width, const unsigned height, const std::string& filename) {
   // Check vector size is correct size
   if (noise.size() != width * height) {
      throw std::invalid_argument("Noise vector size does not match width and height");
   }
   // Check filename provided ends in .ppm
   int len = filename.length();
   if (filename.substr(len - 4) != ".ppm") {
      throw std::invalid_argument("Filename must end in .ppm");
   }

   // Open stream to file
   std::ofstream file(filename);
   if (!file.is_open()) {
      throw std::runtime_error("Could not open file");
   }

   // Write file header
   file << "P3 " << width << " " << height << " 255\n";

   // Write file content
   for (unsigned i = 0; i < height; ++i) {
      for (unsigned j = 0; j < width; ++j) {
         auto index = i * width + j; // 2D to 1D index
         int value = (int) std::round((noise[index] + 1.0) * 127.5); // Scale noise to [0, 255]
         file << value << " " << value << " " << value << " ";
      }
      file << "\n";
   }

   file.close();
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