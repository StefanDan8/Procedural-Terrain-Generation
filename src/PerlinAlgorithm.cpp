#include "PerlinAlgorithm.hpp"
#include "AppConfig.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>

//-----------------------------------------------------------------------------
namespace perlin {
UniformUnitGenerator unifGlbl = UniformUnitGenerator(631);
std::vector<vec2d> gradients2D;

int simpleHash(int i, int j, int N) {
   return (42043 * i + 15299 * j) % N;
}

vec2d random2DGrad() {
   return random2DGrad(AppConfig::getInstance().getUnif());
}

vec2d random2DGrad(UniformUnitGenerator& unif) {
   auto theta = unif.get() * 2 * M_PI;
   return vec2d{cos(theta), sin(theta)};
}

vec3d random3DGrad() {
   return random3DGrad(AppConfig::getInstance().getUnif());
}

vec3d random3DGrad(UniformUnitGenerator& unif) {
   auto cosphi = unif.get() * 2 - 1;
   auto theta = unif.get() * 2 * M_PI;
   auto h = sqrt(1 - cosphi * cosphi);
   return vec3d{h * cos(theta), h * sin(theta), cosphi};
}

double dot(vec2d& x, vec2d& y) {
   return x[0] * y[0] + x[1] * y[1];
}

double dot(vec3d& x, vec3d& y) {
   return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
}

void initialize2DGradients(const int numGradients, UniformUnitGenerator& unif) {
   gradients2D.clear();
   gradients2D.reserve(numGradients);
   for (int i = 0; i < numGradients; i++) {
      gradients2D.emplace_back(random2DGrad(unif));
   }
}

unsigned permutationValue2D(const unsigned chunkX, const unsigned chunkY, const std::vector<unsigned>& permutationTable) {
   const unsigned numGradients = permutationTable.size();
   auto index = permutationTable[(permutationTable[chunkX % numGradients] + chunkY) % numGradients];
   return index;
}

double noise2D(const unsigned x, const unsigned y, const unsigned chunkSize, const unsigned chunkX, const unsigned chunkY, const std::vector<unsigned>& permutationTable) {
   double dx = (x % chunkSize + 1) / (double) chunkSize;
   double dy = (y % chunkSize + 1) / (double) chunkSize;

   vec2d BL = {dx, dy};
   vec2d BR = {dx - 1.0, dy};
   vec2d TL = {dx, dy - 1.0};
   vec2d TR = {dx - 1.0, dy - 1.0};

   int valBL = permutationValue2D(chunkX, chunkY, permutationTable);
   int valBR = permutationValue2D(chunkX + 1, chunkY, permutationTable);
   int valTL = permutationValue2D(chunkX, chunkY + 1, permutationTable);
   int valTR = permutationValue2D(chunkX + 1, chunkY + 1, permutationTable);

   double dotBL = dot(gradients2D.at(valBL), BL);
   double dotBR = dot(gradients2D.at(valBR), BR);
   double dotTL = dot(gradients2D.at(valTL), TL);
   double dotTR = dot(gradients2D.at(valTR), TR);

   // Compute fade curves for x and y
   double u = fade(dx);
   double v = fade(dy);

   // Interpolate the 4 results
   return (lerp(lerp(dotBL, dotBR, u), lerp(dotTL, dotTR, u), v));
}

void fill2DChunk(matrix& resultMatrix, const unsigned chunkSize, const unsigned chunkX, const unsigned chunkY, const std::vector<unsigned>& permutationTable) {
   const unsigned offsetX = chunkSize * chunkX;
   const unsigned offsetY = chunkSize * chunkY;
   for (unsigned i = offsetX; i < offsetX + chunkSize; i++) {
      for (unsigned j = offsetY; j < offsetY + chunkSize; j++) {
         resultMatrix[i][j] = noise2D(i, j, chunkSize, chunkX, chunkY, permutationTable);
      }
   }
}

// very inefficient with all these iterations
/// work in progress
matrix stackNoise(std::vector<matrix>& matrices, std::vector<double>& weights) {
   assert(matrices.size() == weights.size());
   unsigned height = matrices.at(0).size();
   unsigned width = matrices.at(0).at(0).size();
   for (const auto& m : matrices) {
      assert(m.size() == height);
      assert(m.at(0).size() == width);
   }
   // double sum = 0.0;
   // for (const auto& weight : weights) {
   //    sum += weight;
   // }
   // assert(abs(sum - 1.0) <= 1e-10);

   matrix result(height, std::vector<double>(width, 0.0));
   for (unsigned w = 0; w < weights.size(); ++w) {
      for (unsigned i = 0; i < height; ++i) {
         for (unsigned j = 0; j < width; ++j) {
            result[i][j] += weights[w] * matrices[w][i][j];
         }
      }
   }
   return result;
}

//------- General Functions -------

/// @author DB
/// @brief Computes the dot product of two vectors.
/// @param u, v
/// @return result
/// @note Vectors should have the same length. The length doesn't have to be known beforehand.
double dot(std::vector<double> u, std::vector<double> v) {
   assert(u.size() == v.size());

   double result = 0;
   for (unsigned i = 0; i < u.size(); i++) {
      result += u[i] * v[i];
   }
   return result;
}

/// @author DB
/// @brief Permutes (shuffles) the input vector using the given seed.
/// @param arr, seed
/// @note Could also be implemented to instead return a copy and not modify the original vector
/// @note Should be used to permute 0 to 255 values for the Perlin noise algorithm
void shuffleArray(std::vector<unsigned>& arr, const unsigned seed) {
   // Create a random number generator with the given seed
   std::default_random_engine rng(seed);

   // Shuffle the array
   std::shuffle(arr.begin(), arr.end(), rng);
}

/// @author DB
/// @brief Generates a permutation table for the Perlin noise algorithm for the given grid size (default 256).
/// @param seed, gridSize
/// @return permutationTable
/// @note The grid size could be adjusted, but 256 is a common choice
std::vector<unsigned> generatePermutationTable(const unsigned seed, const unsigned gridSize) {
   std::vector<unsigned> permutationTable(gridSize);
   for (unsigned i = 0; i < gridSize; i++) {
      permutationTable[i] = i;
   }
   shuffleArray(permutationTable, seed);
   return permutationTable;
}

/// @author DB
/// @brief A smoothing interpolation function for the Perlin noise algorithm.
/// @param t
/// @return smoothed value
/// @note On [0,1] the function is growing and takes values from 0 to 1
/// @note The derivatives on the edges are 0, so the function makes for a smooth transition
double fade(const double t) {
   return t * t * t * (t * (t * 6 - 15) + 10);
}

/// @author DB
/// @brief The linear interpolation function.
/// @param a, b, t
/// @return the t fraction between the values a and b
double lerp(const double a, const double b, const double t) {
   return a + t * (b - a);
}

//------- 2D Functions -------

/// @author DB
/// @brief Returnes a constant 2D vector based on the given value from the permutation table.
/// @param v
/// @return 2D vector
/// @note The choice of the constant vectors to be returned is arbitrary and could be changed later on - or a grid of constant vectors could instead be computed and used
std::vector<double> getConstVector2D(const unsigned v) {
   // v is the value from the permutation table
   unsigned mod = v % 4;
   switch (mod) {
      case 0:
         return {1, 1};
      case 1:
         return {-1, 1};
      case 2:
         return {-1, -1};
      case 3:
         return {1, -1};
      default:
         return {0, 0};
   }
}

/// @author DB
/// @brief For given coordinates (x,y) returns the value from the permutation table.
/// @param x, y, permutationTable
/// @return An integer from 0 to gridSize-1
/// @note Can be computed in a different way, for example without the last permutation or by hashing
int permutationValue2D(const int x, const int y, const std::vector<int>& permutationTable) {
   const unsigned gridSize = permutationTable.size();
   auto index = permutationTable[(permutationTable[x % gridSize] + y) % gridSize];
   return index;
}

/// @author DB
/// @brief Returnes the Perlin noise value for the given 2D coordinates.
/// @param x, y, permutationTable
/// @return Noise value (double in the range [-1,1])
/// @note
double Noise2D(const double x, const double y, const std::vector<int>& permutationTable) {
   const unsigned gridSize = permutationTable.size();

   // Find the unit square that the point lies in (bottom left corner)
   int X = (int) floor(x) % gridSize;
   int Y = (int) floor(y) % gridSize;

   // Find the fractional coordinates of the point within the square
   double dx = x - floor(x);
   double dy = y - floor(y);

   // Find the vectors pointing to the point from the 4 corners of the square
   // top-bottom / left-right
   std::vector<double> BL = {dx, dy};
   std::vector<double> BR = {dx - 1.0, dy};
   std::vector<double> TL = {dx, dy - 1.0};
   std::vector<double> TR = {dx - 1.0, dy - 1.0};

   // Select a value for each of the 4 corners of the square from the permutation table
   int valBL = permutationValue2D(X, Y, permutationTable);
   int valBR = permutationValue2D(X + 1, Y, permutationTable);
   int valTL = permutationValue2D(X, Y + 1, permutationTable);
   int valTR = permutationValue2D(X + 1, Y + 1, permutationTable);

   // Compute the dot products of the vectors pointing to the point from the 4 corners of the square with the constant vectors
   double dotBL = dot(getConstVector2D(valBL), BL);
   double dotBR = dot(getConstVector2D(valBR), BR);
   double dotTL = dot(getConstVector2D(valTL), TL);
   double dotTR = dot(getConstVector2D(valTR), TR);

   // Compute fade curves for x and y
   double u = fade(dx);
   double v = fade(dy);

   // Interpolate the 4 results
   return (lerp(lerp(dotBL, dotBR, u), lerp(dotTL, dotTR, u), v));
}

//------- 3D Functions -------

/// @author DB
/// @brief Returnes a constant 3D vector based on the given value from the permutation table.
/// @param v
/// @return 3D vector
/// @note The choice of the constant vectors to be returned is arbitrary and could be changed later on - or a grid of constant vectors could instead be computed and used
std::vector<double> getConstVector3D(const unsigned v) {
   // v is the value from the permutation table
   unsigned mod = v % 8;
   switch (mod) {
      case 0:
         return {1, 1, 1};
      case 1:
         return {-1, 1, 1};
      case 2:
         return {-1, -1, 1};
      case 3:
         return {1, -1, 1};
      case 4:
         return {1, 1, -1};
      case 5:
         return {-1, 1, -1};
      case 6:
         return {-1, -1, -1};
      case 7:
         return {1, -1, -1};
      default:
         return {0, 0, 0};
   }
}

/// @author DB
/// @brief For given coordinates (x,y,z) returns the value from the permutation table.
/// @param x, y, z, permutationTable
/// @return An integer from 0 to gridSize-1
/// @note Can be computed in a different way, for example without the last permutation or by hashing
int permutationValue3D(const int x, const int y, const int z, const std::vector<int>& permutationTable) {
   const unsigned gridSize = permutationTable.size();
   return permutationTable[(permutationTable[(permutationTable[x % gridSize] + y) % gridSize] + z) % gridSize];
}

/// @author DB
/// @brief Returnes the Perlin noise value for the given 3D coordinates.
/// @param x, y, z, permutationTable
/// @return Noise value (double in the range [-1,1])
/// @note
double Noise3D(const double x, const double y, const double z, const std::vector<int>& permutationTable) {
   const unsigned gridSize = permutationTable.size();

   // Find the unit cube that the point lies in (bottom left front corner)
   int X = (int) floor(x) % gridSize;
   int Y = (int) floor(y) % gridSize;
   int Z = (int) floor(z) % gridSize;

   // Find the fractional coordinates of the point within the cube
   double dx = x - floor(x);
   double dy = y - floor(y);
   double dz = z - floor(z);

   // Find the vectors pointing to the point from the 8 corners of the cube
   // top-bottom / left-right / front-back
   std::vector<double> BLF = {dx, dy, dz};
   std::vector<double> BRF = {dx - 1.0, dy, dz};
   std::vector<double> BLB = {dx, dy, dz - 1.0};
   std::vector<double> BRB = {dx - 1.0, dy, dz - 1.0};
   std::vector<double> TLF = {dx, dy - 1.0, dz};
   std::vector<double> TRF = {dx - 1.0, dy - 1.0, dz};
   std::vector<double> TLB = {dx, dy - 1.0, dz - 1.0};
   std::vector<double> TRB = {dx - 1.0, dy - 1.0, dz - 1.0};

   // Select a value for each of the 8 corners of the square from the permutation table
   int valBLF = permutationValue3D(X, Y, Z, permutationTable);
   int valBRF = permutationValue3D(X + 1, Y, Z, permutationTable);
   int valBLB = permutationValue3D(X, Y, Z + 1, permutationTable);
   int valBRB = permutationValue3D(X + 1, Y, Z + 1, permutationTable);
   int valTLF = permutationValue3D(X, Y + 1, Z, permutationTable);
   int valTRF = permutationValue3D(X + 1, Y + 1, Z, permutationTable);
   int valTLB = permutationValue3D(X, Y + 1, Z + 1, permutationTable);
   int valTRB = permutationValue3D(X + 1, Y + 1, Z + 1, permutationTable);

   // Compute the dot products of the vectors pointing to the point from the 8 corners of the cube with the constant vectors
   double dotBLF = dot(getConstVector3D(valBLF), BLF);
   double dotBRF = dot(getConstVector3D(valBRF), BRF);
   double dotBLB = dot(getConstVector3D(valBLB), BLB);
   double dotBRB = dot(getConstVector3D(valBRB), BRB);
   double dotTLF = dot(getConstVector3D(valTLF), TLF);
   double dotTRF = dot(getConstVector3D(valTRF), TRF);
   double dotTLB = dot(getConstVector3D(valTLB), TLB);
   double dotTRB = dot(getConstVector3D(valTRB), TRB);

   // Compute fade curves for x, y and z
   double u = fade(dx);
   double v = fade(dy);
   double w = fade(dz);

   // Interpolate the 8 results
   double a = lerp(dotBLF, dotBRF, u);
   double b = lerp(dotBLB, dotBRB, u);
   double c = lerp(dotTLF, dotTRF, u);
   double d = lerp(dotTLB, dotTRB, u);
   double e = lerp(a, b, v);
   double f = lerp(c, d, v);
   return (lerp(e, f, w));
}

void printMatrix(const matrix& matrix) {
   for (const auto& row : matrix) {
      for (const auto& value : row) {
         std::cout << std::fixed << std::setprecision(2) << value << " ";
      }
      std::cout << std::endl;
   }
}

}