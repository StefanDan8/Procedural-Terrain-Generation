#include "Renderer.hpp"

#include <fstream>
#include <stdexcept>
#include <cmath>

// -----------------------------------------------------------------------------
/* The goal here is to render the results from the Perlin noise generator.
   To do so, we will first write to a NetPBM file, and then use something to convert it into a .png file.
   For now, we will only render a 2D image.
 */
namespace render {
    void ppm(const std::vector<double>& noise, const unsigned width, const unsigned height, const std::string& filename) {
        // Check vector size is correct size
        if (noise.size() != width * height) {
            throw std::invalid_argument("Noise vector size does not match width and height");
        }
        // Check filename provided ends in .ppm
        int len = filename.length();
        if (filename.substr(len - 4) != ".ppm") {
            throw std::invalid_argument("Filename must end in .ppm");
        }

        // Open stream to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        // Write file header
        file << "P3 " << width << " " << height << " 255\n";

        // Write file content
        for (unsigned i = 0; i < height; ++i) {
            for (unsigned j = 0; j < width; ++j) {
                auto index = i * width + j; // 2D to 1D index
                int value = (int) std::round((noise[index] + 1.0) * 127.5); // Scale noise to [0, 255]
                file << value << " " << value << " " << value << " ";
            }
            file << "\n";
        }

        file.close();
    }
}