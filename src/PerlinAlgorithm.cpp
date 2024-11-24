#include "PerlinAlgorithm.hpp"
#include <random>
#include <algorithm>
#include <cassert>
#include <cmath>
//-----------------------------------------------------------------------------
namespace perlin {
/// @author SD
/// @brief Generates a normalized 3D vector with non-negative values.
/// @param seed
/// @return
/// @note This is just a test function. Not to be used in production,
std::array<double, 3> random3DVector(int seed) {
   std::mt19937 generator(seed);
   std::uniform_real_distribution<double> unif(0.0, 1.0);
   auto x = unif(generator);
   auto y = unif(generator);
   auto z = unif(generator);
   auto length = sqrt(x * x + y * y + z * z);
   return std::array<double, 3>{x / length, y / length, z / length};
}

//------- General Functions -------

/// @author DB
/// @brief Computes the dot product of two vectors. 
/// @param u, v
/// @return result
/// @note Vectors should have the same length. The length doesn't have to be known beforehand.
double dot(std::vector<double> u, std::vector<double> v) {
   //Define the macro for assert
   #define ASSERT(condition, message) \
      do { \
         assert(condition && #message); \
      } while (0)
   ASSERT(u.size() == v.size(), "Vectors must have the same length");

    double result = 0;
    for (int i = 0; i < u.size(); i++) {
        result += u[i] * v[i];
    }
    return result;
}

/// @author DB
/// @brief Permutes (shuffles) the input vector using the given seed. 
/// @param arr, seed
/// @note Could also be implemented to instead return a copy and not modify the original vector
/// @note Should be used to permute 0 to 255 values for the Perlin noise algorithm
void shuffleArray(std::vector<int>& arr, const unsigned seed) {
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
std::vector<int> generatePermutationTable(const unsigned seed, const unsigned gridSize = 256) {
    std::vector<int> permutationTable(gridSize);
    for (unsigned i = 0; i < gridSize; i++) {
        permutationTable[i] = i;
    }
    shuffleArray(permutationTable, seed);
    return permutationTable;
}

/// @author DB
/// @brief A smoothing interpolation function for the Perlin noise algorithm. 
/// @param t
/// @return
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
/// @note The choice of the constant vectors to be returned is arbitrary and could be changed later on
std::vector<double> getConstVector2D(const unsigned v){
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
int permutationValue(const int x, const int y, const std::vector<int>& permutationTable){
   const unsigned gridSize = permutationTable.size();
   return permutationTable[(permutationTable[x % gridSize] + y) % gridSize];
}

/// @author DB
/// @brief Returnes the Perlin noise value for the given 2D coordinates. 
/// @param x, y, permutationTable
/// @return Noise value (double in the range [-1,1])
/// @note
double Noise2D(const double x, const double y, const std::vector<int>& permutationTable) {

   const unsigned gridSize = permutationTable.size();

   // Find the unit square that the point lies in (bottom left corner)
   int X = (int)floor(x) % gridSize;
   int Y = (int)floor(y) % gridSize;

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
   int valBL = permutationValue(X, Y, permutationTable);
   int valBR = permutationValue(X + 1, Y, permutationTable);
   int valTL = permutationValue(X, Y + 1, permutationTable);
   int valTR = permutationValue(X + 1, Y + 1, permutationTable);

   // Compute the dot products of the vectors pointing to the point from the 4 corners of the square with the constant vectors
   double dotBL = dot(getConstVector2D(valBL), BL);
   double dotBR = dot(getConstVector2D(valBR), BR);
   double dotTL = dot(getConstVector2D(valTL), TL);
   double dotTR = dot(getConstVector2D(valTR), TR);

   // Compute fade curves for x and y
   double u = fade(dx);
   double v = fade(dy);

   // Interpolate the 4 results
   return(lerp(lerp(dotBL, dotBR, u), lerp(dotTL, dotTR, u), v));
}

//------- 3D Functions -------
}