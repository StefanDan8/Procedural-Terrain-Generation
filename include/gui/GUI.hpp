#ifndef GUI_CLASS_HPP
#define GUI_CLASS_HPP

#include "Window.hpp"

class GUI {
   public:
   GUI(Window& window);

   void NewFrame();

   void Shutdown();
   void RenderDrawData();

   private:
   Window& window;
};
#endif