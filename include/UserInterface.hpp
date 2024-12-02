#include <atomic>
#include <thread>

namespace gui {
/**
* @author PK
* @brief Structure to hold the parameters for the Perlin noise generation.
*/
struct parameters {
   unsigned seed;
   unsigned chunkSize;
   unsigned nChunksX;
   unsigned nChunksY;
};

/**
* @author PK
* @brief Enum for the output type. (FILE, PPM, PNG, OBJ)
*/
enum output_type { FILE,
                   PPM,
                   PNG,
                   OBJ };

/**
* @author PK
* @brief Asks the user if they want to use custom parameters or not.
* @returns True if user wants to insert their own values ("n")
*/
bool use_custom();

/**
* @author PK
* @brief Asks the user for the custom parameters.
* @returns parameters struct, containing seed, chunkSize, nChunksX and nChunksY.
*/
parameters get_custom_parameters();

/**
* @author PK
* @brief Returns the default parameters
* @returns parameters struct, containing seed (42), chunkSize (16), nChunksX (16) and nChunksY (16).
*/
parameters get_default_parameters();

/**
* @author PK
* @brief Creates a thread that prints a progress bar.
* @param done Atomic boolean to stop the thread once true.
* @returns The thread, .join() it to stop it.
*/
std::thread progress(std::atomic<bool>& done);

/**
* @author PK
* @brief Asks the user for the output method.
* @returns The output type (FILE, PPM, PNG, OBJ)
*/
output_type get_output_method();

/**
* @author PK
* @brief Asks the user for the file path.
* @returns The file path as a string.
* @note No checks are done to check if writing here will be successful.
*/
std::string get_file_path();

}
