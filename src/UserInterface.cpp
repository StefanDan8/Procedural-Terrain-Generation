#include "UserInterface.hpp"

#include <iostream>

//-----------------------------------------------------------------------------
namespace gui {

int get_positive_number(std::string prompt) {
   int num = -1;

   // Trailing whitespace will not work within CLion.
   if (prompt.substr(prompt.length() - 1) != " ") {
      prompt += " ";
   }

   while (num < 0) {
      std::cout << std::endl << prompt; // Without std::endl, the prompt can sometimes have a random line break.
      std::cin >> num;

      if (std::cin.fail()) { // Not a number
         std::cin.clear();
         std::cout << "Please enter a valid number." << std::endl;

         num = -1;
      } else if (num < 1) { // Negative (or zero) number
         std::cout << "Please enter a positive (above zero) number." << std::endl;

         num = -1;
      }
   }

   return num;
}

}
