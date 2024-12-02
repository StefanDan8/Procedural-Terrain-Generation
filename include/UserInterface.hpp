#include <atomic>
#include <thread>

namespace gui {
/**
* @author PK
* @brief Structure to hold the parameters for the Perlin noise generation.
*/
struct parameters {
  unsigned chunkSize;
  unsigned nChunksX;
  unsigned nChunksY;
};

enum output_type { FILE, PPM, PNG };

bool use_custom();
parameters get_custom_parameters();
parameters get_default_parameters();
std::thread progress(std::atomic<bool> &done);
output_type get_output_method();
std::string get_file_path();

}
