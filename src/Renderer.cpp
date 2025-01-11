#include "Renderer.hpp"

#include "lodepng/lodepng.h"
#include <cmath>
#include <fstream>
#include <stdexcept>

#include "PerlinAlgorithm.hpp"

namespace render {
void create_ppm(const std::vector<std::vector<int>>& noise, const std::string& filename) {
   // Check vector size is correct size
   const unsigned width = noise.size();
   const unsigned height = noise[0].size();
   if (width < 1 || height < 1) {
      throw std::invalid_argument("Width and height must be greater than 0");
   }

   // Check filename provided ends in .ppm
   const unsigned len = filename.length();
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
   unsigned len = filename.length();
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
   if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
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

std::vector<std::vector<double>> normalizePlusMinusOne(const std::vector<std::vector<double>>& matrix) {
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

   // Normalize the matrix to [-1,1]
   std::vector<std::vector<double>> normalized(rows, std::vector<double>(cols));
   for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < cols; ++j) {
         normalized[i][j] =
            2.0f * (matrix[i][j] - minVal) / (maxVal - minVal) - 1.0f;
      }
   }
   return normalized;
}

std::vector<std::vector<double>> normalizeUnit(const std::vector<std::vector<double>>& matrix, const double sumWeight) {
   size_t rows = matrix.size();
   size_t cols = matrix[0].size();

   // Normalize the matrix to [-1,1]
   std::vector<std::vector<double>> normalized(rows, std::vector<double>(cols));
   for (size_t i = 0; i < rows; ++i) {
      for (size_t j = 0; j < cols; ++j) {
         //normalized[i][j] = 2 * matrix[i][j] / (rows + cols);
         normalized[i][j] = matrix[i][j] / sumWeight;
      }
   }
   return normalized;
}

std::vector<std::vector<int>> normalizeMatrixReLU(const std::vector<std::vector<double>>& matrix, int threshold) {
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
         normalized[i][j] = std::max(threshold, static_cast<int>(255 * (matrix[i][j] - minVal) / (maxVal - minVal)));
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

void ReLU(perlin::matrix& matrix, double threshold) {
   for (auto& row : matrix) {
      for (auto& el : row) {
         el = std::max(el, threshold);
      }
   }
}

void Max(perlin::matrix& matrix, perlin::matrix& filter) {
   for (unsigned i = 0; i < matrix.size(); i++) {
      for (unsigned j = 0; j < matrix[0].size(); j++) {
         matrix[i][j] = std::max(matrix[i][j], filter[i][j]);
      }
   }
}

void createPolyMesh(const perlin::matrix& matrix, unsigned width, unsigned height, const std::string& filename) {
   unsigned numX = matrix.size() - 1;
   unsigned numY = matrix[0].size() - 1;
   unsigned numVertices = (numX + 1) * (numY + 1);
   std::vector<perlin::vec3d> vertices(numVertices);
   std::vector<perlin::vec2d> textureCoord(numVertices);
   std::vector<perlin::vec3d> normals(numVertices);
   double invNumX = 1.0 / numX;
   double invNumY = 1.0 / numY;

   for (unsigned j = 0; j <= numY; ++j) {
      for (unsigned i = 0; i <= numX; ++i) {
         vertices[j * (numX + 1) + i] = perlin::vec3d{width * (i * invNumX - 0.5), matrix[i][j], height * (j * invNumY - 0.5)};
         textureCoord[j * (numX + 1) + i] = perlin::vec2d{i * invNumX, j * invNumY};
      }
   }

   unsigned numFaces = numX * numY;
   std::vector<unsigned> faces(numFaces, 4);
   std::vector<unsigned> verticesVector(numFaces * 4);
   for (unsigned j = 0, k = 0; j < numY; ++j) {
      for (unsigned i = 0; i < numX; ++i) {
         verticesVector[k] = j * (numX + 1) + i;
         verticesVector[k + 1] = j * (numX + 1) + i + 1;
         verticesVector[k + 2] = (j + 1) * (numX + 1) + i + 1;
         verticesVector[k + 3] = (j + 1) * (numX + 1) + i;
         k += 4;
      }
   }

   // export to .obj
   std::ofstream ofs;
   ofs.open("../output/" + filename, std::ios_base::out);
   if (!ofs.is_open()) {
      std::cerr << "Failed to open file. Ensure ../output exists and is writable." << std::endl;
   }
   for (unsigned i = 0; i < numVertices; ++i) {
      ofs << "v " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << std::endl;
   }
   for (unsigned i = 0; i < numVertices; ++i) {
      ofs << "vt " << textureCoord[i][0] << " " << textureCoord[i][1] << std::endl;
   }
   for (unsigned i = 0; i < numVertices; ++i) {
      ofs << "vn " << normals[i][0] << " " << normals[i][1] << " " << normals[i][2] << std::endl;
   }
   for (unsigned i = 0, k = 0; i < numFaces; ++i) {
      ofs << "f ";
      for (unsigned j = 0; j < faces[i]; ++j) {
         unsigned objIndex = verticesVector[k + j] + 1;
         ofs << objIndex << "/" << objIndex << "/" << objIndex << ((j == (faces[i] - 1)) ? "" : " ");
      }
      ofs << std::endl;
      k += faces[i];
   }
   ofs.close();
}
}