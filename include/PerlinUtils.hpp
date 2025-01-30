#ifndef PERLIN_UTILS_HPP
#define PERLIN_UTILS_HPP

// --- Global Libraries ---

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iomanip> // For std::fixed and std::setprecision
#include <iostream>
#include <random>
#include <optional>
#include <execution>
#include <chrono>

#include "AppConfig.hpp"

namespace perlin {

// --- Common Structures ---

/// @brief 2D normalized real vector
using vec2d = std::array<double, 2>;

/// @brief 3D normalized real vector
using vec3d = std::array<double, 3>;

/// @brief Matrix with real values.
using matrix = std::vector<std::vector<double>>;

/// @brief 3D tensor with real values.
using tensor = std::vector<std::vector<std::vector<double>>>;

// --- General Vector Functions ---

/// @param i, j two input numbers, meant to be 2D coordinates
/// @param N modulo value
int simpleHash(int i, int j, int N);

/// @brief Generates a random 2D normalized vector
vec2d random2DGrad();

/// @brief Generates a random 3D normalized vector
vec3d random3DGrad();

/// @author SD
/// @brief Generates a random 2D normalized vector
/// @param generator A random number generator for Unif[0,1]
vec2d random2DGrad(UniformUnitGenerator& generator);

/// @author SD
/// @brief Generates a random 3D normalized vector
/// @param generator A random number generator for Unif[0,1]
vec3d random3DGrad(UniformUnitGenerator& generator);

/// @brief Computes the dot product of two 2D vectors
double dot(const vec2d& x, const vec2d& y);

/// @brief Computes the dot product of two 3D vectors
double dot(const vec3d& x, const vec3d& y);

/// @brief Computes the fade curve for Perlin noise. It is a growing smooth function mapping [0,1] to [0,1]
double fade(const double t);

/// @brief A linear interpolation function
/// @param a, b values of the interval [a,b]
/// @param t fraction of the distance from a to b
double lerp(const double a, const double b, const double t);

}

#endif // PERLIN_UTILS_HPP