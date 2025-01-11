#include "UserInterface.hpp"

#include <algorithm>
#include <iostream>

//-----------------------------------------------------------------------------
namespace gui {
// Private functions
namespace {

/**
   * @author PK
   * @brief Returns input string in lowercase.
   * @param input String to convert to lowercase.
   * @return Input string in lowercase.
   *
   * @private
   */
std::string toLower(std::string input) {
   std::transform(input.begin(), input.end(), input.begin(), ::tolower);
   return input;
}

/**
   * @author PK
   * @brief Gets a positive number from the user, using stdin and stdout, performing checks to ensure the input is valid.
   * @param prompt Message to display to the user.
   * @return Number entered by the user.
   *
   * @private
   */
int get_positive_number(std::string prompt) {
   int num = -1;

   // Trailing whitespace will not work within CLion.
   if (prompt.substr(prompt.length() - 1) != " ") {
      prompt += " ";
   }

   int tries = 0;

   while (num < 0 && tries < 3) {
      std::cout << prompt; // Without std::endl, the prompt can sometimes have a random line break.
      std::cin >> num;

      if (std::cin.fail()) { // Not a number
         std::cin.clear();
         std::cout << "Please enter a valid number." << std::endl;

         num = -1;
         tries++;
      } else if (num < 1) { // Negative (or zero) number
         std::cout << "Please enter a positive (above zero) number." << std::endl;

         num = -1;
         tries++;
      }
   }

   if (tries == 3) {
      std::cout << "Too many invalid inputs. Exiting..." << std::endl;
      exit(1);
   }

   return num;
}
}

bool use_custom() {
   std::string input;
   std::cout << "Welcome to the Perlin Noise generator!" << std::endl;
   std::cout << "Would you like to use the default parameters? (y/n): " << std::flush;

   std::cin >> input;
   std::string lower = toLower(input);
   int tries = 0;

   while (lower != "y" && lower != "n" && tries < 3) {
      std::cout << "Invalid input. Please enter 'y' or 'n': " << std::flush;
      tries++;

      std::cin >> input;
      lower = toLower(input);
   }

   if (tries == 3) {
      std::cout << "Too many invalid inputs. Exiting..." << std::endl;
      exit(1);
   }

   return lower == "n";
}

parameters get_custom_parameters() {
   parameters params = {0, 0, 0, 0};

   std::cout << "Seed: " << params.seed << ", Chunk Size: " << params.chunkSize << ", chunksX: " << params.nChunksX << ", chunksY: " << params.nChunksY << std::endl;
   params.seed = get_positive_number("Enter the seed: ");

   std::cout << "Seed: " << params.seed << ", Chunk Size: " << params.chunkSize << ", chunksX: " << params.nChunksX << ", chunksY: " << params.nChunksY << std::endl;
   params.chunkSize = get_positive_number("Enter the chunk size: ");

   std::cout << "Seed: " << params.seed << ", Chunk Size: " << params.chunkSize << ", chunksX: " << params.nChunksX << ", chunksY: " << params.nChunksY << std::endl;
   params.nChunksX = get_positive_number("Enter the number of chunks in the X direction: ");

   std::cout << "Seed: " << params.seed << ", Chunk Size: " << params.chunkSize << ", chunksX: " << params.nChunksX << ", chunksY: " << params.nChunksY << std::endl;
   params.nChunksY = get_positive_number("Enter the number of chunks in the Y direction: ");

   return params;
}

parameters get_default_parameters() { return {42, 16, 16, 16}; }

std::thread progress(std::atomic<bool>& done) {
   std::cout << "Generating." << std::flush;

   return std::thread([&done]() {
      // Anonymous function to run in the thread.
      while (!done) {
         std::cout << "." << std::flush;
         std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      std::cout << std::endl;
   });
}

output_type get_output_method() {
   std::string input;
   std::cout << "Done! How would you like to save the output? (file/ppm/png/obj): " << std::flush;

   std::cin >> input;
   std::string lower = toLower(input);
   int tries = 0;

   while (lower != "file" && lower != "ppm" && lower != "png" && lower != "obj" && tries < 3) {
      std::cout << "Invalid input. Please enter 'file', 'ppm', 'png' or 'obj': " << std::flush;
      tries++;

      std::cin >> input;
      lower = toLower(input);
   }

   if (tries == 3) {
      std::cout << "Too many invalid inputs. Exiting..." << std::endl;
      exit(1);
   }

   return (lower == "file") ? FILE : (lower == "ppm") ? PPM :
      (lower == "png")                                ? PNG :
                                                        OBJ;
}

std::string get_file_path() {
   std::string input;
   std::cout << "And where would you like to save the file (include the extension please!): " << std::flush;

   std::cin >> input;
   return input; // We can't really check this, not without a lot of effort for now anyway.
}

}
