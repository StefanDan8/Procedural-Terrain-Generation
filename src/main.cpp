#include "PerlinNoise.hpp"
#include "Renderer.hpp"
#include "UserInterface.hpp"

#include <atomic>
#include <iostream>

int main() {
   const bool useCustomValues = gui::use_custom();
   const gui::parameters params = useCustomValues ? gui::get_custom_parameters() : gui::get_default_parameters();

   auto& [seed, chunkSize, nChunksX, nChunksY] = params; // Structured binding, gets the values from the struct.

   // I would rather have put this in the progress function, but I couldn't get it to work.
   // This small section is responsible for initializing the progress "bar".
   std::atomic<bool> done(false);
   std::thread progressThread = gui::progress(done);

   perlin::UniformUnitGenerator randomG(seed);
   // Perlin noise generation
   perlin::matrix result(nChunksX * chunkSize, std::vector<double>(nChunksY * chunkSize, 0.0));

   // perlin::initialize2DGradients(chunkSize, randomG);

   // const std::vector<unsigned> permutationTable = perlin::generatePermutationTable(seed, chunkSize);
   // for (unsigned i = 0; i < nChunksX; i++) {
   //    for (unsigned j = 0; j < nChunksY; j++) {
   //       perlin::fill2DChunk(result, chunkSize, i, j, permutationTable);
   //    }
   // }

   // Stop progress bar
   done = true;
   progressThread.join();

   // Handling outputting the result
   const gui::output_type outputMethod = gui::get_output_method();
   const std::string outputFileName = gui::get_file_path();

   switch (outputMethod) {
      case gui::output_type::FILE:
         render::writeMatrixToFile(render::normalizeMatrix(result), outputFileName);
         break;
      case gui::output_type::PPM:
         render::create_ppm(render::normalizeMatrix(result), outputFileName);
         break;
      case gui::output_type::PNG:
         render::create_png(render::normalizeMatrix(result), outputFileName);
         break;
      case gui::output_type::OBJ:
         render::createPolyMesh(result, 10, 10, outputFileName);
         break;
   }

   std::cout << "Saved! Goodbye!" << std::endl;

   return 0;
}