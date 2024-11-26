#include <array>
#include <vector>

namespace perlin {
std::array<double, 3> random3DVector(int seed = 631);

//------- General Functions -------
double dot(std::vector<double> u, std::vector<double> v);

void shuffleArray(std::vector<int>& arr, const unsigned seed);

std::vector<int> generatePermutationTable(const unsigned seed, const unsigned gridSize = 256);

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