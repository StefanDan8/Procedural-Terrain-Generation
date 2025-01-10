#ifndef RENDEREROOP_H
#define RENDEREROOP_H

#include <string>
#include <vector>

/*
 * Collection of functions related to rendering and exporting noise values, now bundled into a class.
 * @author PK
 *
 */
class Renderer {
   public:
   /**
   * Create a NetPBM (.ppm) file
   * @author PK
   * @param noise Normalised noise to generate image from
   * @param filename Filename (and path) to save to, filename must end in .ppm. Defaults to "output.ppm"
   * @note Provided noise SHOULD be normalized first, else this method may fail.
   */
   static void create_ppm(const std::vector<std::vector<int>>& noise, const std::string& filename = "output.ppm");
};

#endif //RENDEREROOP_H
