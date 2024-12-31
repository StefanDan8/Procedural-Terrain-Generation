#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#ifndef RENDER_H
#define RENDER_H

// Collection of functions related to rendering (or exporting) noise
namespace render {
/**
* @author PK
* @brief Create a NetPBM file (.ppm) with the provided noise.
* @param noise Normalized noise to generate image from.
* @param filename Filename (and path) to save to, filename must end in .ppm. Defaults to "output.ppm"
* @note Provided noise SHOULD be normalized first, else this procedure may fail.
*/
void create_ppm(const std::vector<std::vector<int>>& noise, const std::string& filename = "output.ppm");

/**
* @author PK
* @brief Creates a Portable Network Graphic file (.png) with the provided noise.
* @param noise Normalized noise to generate image from.
* @param filename Filename (and path) to save to, filename must end in .png. Defaults to "output.png"
* @note Provided noise SHOULD be normalized first, else this procedure may fail.
* @pre lodepng.h must be included.
*/
void create_png(const std::vector<std::vector<int>>& noise, const std::string& filename = "output.png");

/// @author SD
/// @brief Normalize a matrix of doubles to integers in the range [0, 255]
/// @param matrix
/// @return a matrix of integers in range [0, 255]
std::vector<std::vector<int>> normalizeMatrix(const std::vector<std::vector<double>>& matrix);

std::vector<std::vector<double>> normalizePlusMinusOne(const std::vector<std::vector<double>>& matrix);

std::vector<std::vector<double>> normalizeUnit(const std::vector<std::vector<double>>& matrix);

/// @author SD
/// @brief Normalize a matrix of doubles to integers in the range [threshold, 255]
/// @param matrix
/// @return a matrix of integers in range [threshold, 255]
std::vector<std::vector<int>> normalizeMatrixReLU(const std::vector<std::vector<double>>& matrix, int threshold);

void ReLU(std::vector<std::vector<double>>& matrix, double threshold);

void Max(std::vector<std::vector<double>>& matrix, std::vector<std::vector<double>>& filter);

/// @author SD
/// @brief Writes an integer matrix to a .txt file which is stored in the `output` folder.
/// @param matrix matrix to be written to file
/// @param filename name of the file
void writeMatrixToFile(const std::vector<std::vector<int>>& matrix, const std::string& filename);

/// @author SD
/// @brief Transforms a matrix to a polygonal mesh with square faces, where each matrix element is a node, whose Z-coordinate in the mesh is its value
/// @param matrix matrix to be written to file
/// @param width width of the resulting mesh (X-coord)
/// @param height height of the resulting mesh (Y-coord)
/// @param filename name of the file where the result is saved
void createPolyMesh(const std::vector<std::vector<double>>& matrix, unsigned width, unsigned height, const std::string& filename);

}
#endif