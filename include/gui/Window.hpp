#ifndef WINDOW_CLASS_HPP
#define WINDOW_CLASS_HPP

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

class Camera2D;
class Window;

/// @brief Introduced for managing multiple callbacks on same glfw window, as it can have only one parent. If any other callbacks would be added, put a pointer here.
struct CallbackContext {
   Window* window = nullptr;
   Camera2D* camera2D = nullptr;
};

class Window {
   public:
   CallbackContext context;
   Window(const unsigned width, const unsigned height);

   void setViewport();

   void setFrameBufferWidth(const int width) {
      frameBufferWidth = width;
   }
   void setFrameBufferHeight(const int height) {
      frameBufferHeight = height;
   }

   GLFWwindow* getWindow() {
      return window;
   }

   float* getRenderWidth() {
      return &renderWidth;
   }
   float* getRenderHeight() {
      return &renderHeight;
   }

   int getFrameBufferWidth() {
      return frameBufferWidth;
   }
   int getFrameBufferHeight() {
      return frameBufferHeight;
   }

   bool isActive();
   void SwapBuffers();
   void Delete();

   private:
   static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
   int frameBufferWidth = 0;
   int frameBufferHeight = 0;
   float renderWidth = 0.0f;
   float renderHeight = 0.0f;
   GLFWwindow* window = nullptr;
};
#endif