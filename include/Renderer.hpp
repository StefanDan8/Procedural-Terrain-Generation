#include <string>
#include <vector>

#ifndef RENDER_H
#define RENDER_H
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
namespace render {

void ppm(const std::vector<double>& noise, const unsigned width, const unsigned height, const std::string& filename = "output.ppm");

/// @author SD
/// @brief Normalize a matrix of doubles to integers in the range [0, 255]
/// @param matrix
/// @return a matrix of integers in range [0, 255]
std::vector<std::vector<int>> normalizeMatrix(const std::vector<std::vector<double>>& matrix);

/// @author SD
/// @brief Writes an integer matrix to a .txt file which is stored in the `output` folder.
/// @param matrix matrix to be written to file
/// @param filename name of the file
void writeMatrixToFile(const std::vector<std::vector<int>>& matrix, const std::string& filename);
}
#endif