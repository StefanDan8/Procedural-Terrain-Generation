#ifndef WINDOW_CLASS_HPP
#define WINDOW_CLASS_HPP

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

class Window {
   public:
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

   float* getImguiWidth() {
      return &imguiWidth;
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
   float imguiWidth = 0.0f;
   float renderWidth = 0.0f;
   float renderHeight = 0.0f;
   GLFWwindow* window = nullptr;
};
#endif