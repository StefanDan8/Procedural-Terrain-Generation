
#include "AppConfig.hpp"
#include "PerlinOOP.hpp"
#include "Renderer.hpp"
#include "UserInterface.hpp"

int main() {
   perlin::AppConfig::initialize(42);
   const unsigned sizeX = 360;
   const unsigned sizeY = 360;
   std::vector<std::pair<unsigned, double>> params{std::make_pair(256, 1), std::make_pair(128, 0.5), std::make_pair(64, 0.25), std::make_pair(32, 0.125), std::make_pair(16, 0.0625), std::make_pair(8, 0.03)};
   std::vector<std::pair<unsigned, double>> params2{std::make_pair(360, 60), std::make_pair(180, 80), std::make_pair(120, 70),
                                                    std::make_pair(90, 125), std::make_pair(45, 40), std::make_pair(12, 30), std::make_pair(8, 15)};
   // std::vector<std::pair<unsigned, double>> params3{std::make_pair(360, 60), std::make_pair(180, 120), std::make_pair(120, 100),
   //                                                  std::make_pair(90, 60), std::make_pair(45, 30), std::make_pair(12, 15), std::make_pair(8, 6), std::make_pair(3, 2)};
   //render::ReLU(result, -10);
   std::vector<std::pair<unsigned, double>> params4{std::make_pair(360, 60), std::make_pair(180, 300), std::make_pair(120, 60),
                                                    std::make_pair(90, 40), std::make_pair(45, 20), std::make_pair(12, 10), std::make_pair(8, 5), std::make_pair(3, 2)};

   perlin::PerlinNoise2D noise = perlin::PerlinNoise2D(sizeX, sizeY, params4);
   perlin::matrix result(sizeX, std::vector<double>(sizeY, 0.0));
   noise.fill(result);
   //render::writeMatrixToFile(render::normalizeMatrix(result), "oop.txt");

   //render::ReLU(result, -10);
   perlin::matrix filter(sizeX, std::vector<double>(sizeY, 0.0));
   std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 10), std::make_pair(120, 10), std::make_pair(60, 10), std::make_pair(30, 1)};
   perlin::PerlinNoise2D noiseFilter(sizeX, sizeY, filterParams);
   noiseFilter.fill(filter);

   render::Max(result, filter);
   render::create_png(render::normalizeMatrix(result), "oop46.png");
   render::createPolyMesh(result, 360, 360, "oop46.obj");
   // render::writeMatrixToFile(render::normalizeMatrix(result), "stacked.txt");
   // render::createPolyMesh(result, 4, 4, "stacked.obj");
}