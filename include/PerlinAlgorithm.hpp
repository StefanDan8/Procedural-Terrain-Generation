
#ifndef _PERLIN_HPP
#define _PERLIN_HPP

#include <array>
#include <iomanip> // For std::fixed and std::setprecision
#include <iostream>
#include <random>
#include <vector>

namespace perlin {

/// @author SD
/// @brief Random number generator for Unif[0.0, 1.0]
class UniformUnitGenerator {
   public:
   UniformUnitGenerator(unsigned seed) : generator(seed), distribution(0.0, 1.0) {}

   double get() {
      return distribution(generator);
   }

   private:
   std::mt19937 generator;
   std::uniform_real_distribution<double> distribution;
};

/// @brief 2D normalized real vector
using vec2d = std::array<double, 2>;
/// @brief 3D normalized real vector
using vec3d = std::array<double, 3>;
/// @brief Matrix with real values.
using matrix = std::vector<std::vector<double>>;
/// @brief 3D tensor with real values.
using tensor = std::vector<std::vector<std::vector<double>>>;

/// @brief Serves as global random number generator for Unif[0,1]
extern UniformUnitGenerator unifGlbl;
/// @brief Gradients for 2D generation. Assumed to have length = chunkSize
extern std::vector<vec2d> gradients2D;

/// @author SD
/// @brief Generates a random 2D normalized vector
/// @param generator A random number generator for Unif[0,1]
vec2d random2DGrad(UniformUnitGenerator& generator = unifGlbl);

/// @author SD
/// @brief Generates a random 3D normalized vector
/// @param generator A random number generator for Unif[0,1]
vec3d random3DGrad(UniformUnitGenerator& generator = unifGlbl);

double dot(vec2d& x, vec2d& y);

double dot(vec3d& x, vec3d& y);

/// @author SD
/// @brief Initialize the global vector of 2D gradients `gradients2D`.
/// @param numGradients How many gradients we consider
/// @param unif A random number generator for Unif[0,1]
void initialize2DGradients(const int numGradients, UniformUnitGenerator& unif = unifGlbl);

/// @author SD
/// @brief Compute the Perlin Noise value of a pixel.
/// @param x x-coordinate within chunk
/// @param y y-coordinate within chunk
/// @param chunkSize the size of a chunk
/// @param chunkX x-coordinate of the chunk within the entire grid
/// @param chunkY y-coordinate of the chunk within the entire grid
/// @param permutationTable the permutation table used to pick gradients
/// @return the Perlin Noise value of the pixel result[chunkX*chunkSize+x][chunkY*chunkSize+y]
double noise2D(const unsigned x, const unsigned y, const unsigned chunkSize, const unsigned chunkX, const unsigned chunkY, const std::vector<unsigned>& permutationTable);

/// @author SD
/// @brief Fill a chunk with Perlin Noise Values.
/// @param resultMatrix the final output -- to be filled
/// @param chunkSize the size of a chunk
/// @param chunkX x-coordinate of the chunk within the entire grid
/// @param chunkY y-coordinate of the chunk within the entire grid
/// @param permutationTable the permutation table used to pick gradients
void fill2DChunk(matrix& resultMatrix, const unsigned chunkSize, const unsigned chunkX, const unsigned chunkY, const std::vector<unsigned>& permutationTable);

/// @author SD
/// @brief Prints the values of `matrix` to standard output stream
/// @param matrix matrix to be printed
void printMatrix(const matrix& matrix);

/// @author SD
/// @brief Return the index corresponding to the 2D gradient for the corner of the grid given by `chunkX` and `chunkY`.
/// @param chunkX x-coordinate of the chunk within the entire grid
/// @param chunkY y-coordinate of the chunk within the entire grid
/// @param chunkSize the size of a chunk
/// @param permutationTable the permutation table used to pick gradients
unsigned permutationValue2D(const unsigned chunkX, const unsigned chunkY, const std::vector<unsigned>& permutationTable);

/// work in  progress
matrix stackNoise(std::vector<matrix>& matrices, std::vector<double>& weights);

//------- General Functions -------
double dot(std::vector<double> u, std::vector<double> v);

void shuffleArray(std::vector<unsigned>& arr, const unsigned seed);

std::vector<unsigned> generatePermutationTable(const unsigned seed, const unsigned gridSize = 256);

double fade(const double t);

double lerp(const double a, const double b, const double t);

//------- 2D Functions -------
std::vector<double> getConstVector2D(const unsigned v);

int permutationValue2D(const int x, const int y, const std::vector<int>& permutationTable);

double Noise2D(const double x, const double y, const std::vector<int>& permutationTable);

//------- 3D Functions -------
std::vector<double> getConstVector3D(const unsigned v);

int permutationValue3D(const int x, const int y, const int z, const std::vector<int>& permutationTable);

double Noise3D(const double x, const double y, const double z, const std::vector<int>& permutationTable);
}
#endif
