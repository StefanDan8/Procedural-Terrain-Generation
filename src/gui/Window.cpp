#include "Window.hpp"

Window::Window(const unsigned width, const unsigned height) {
   glfwInit();

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   window = glfwCreateWindow(width, height, "Group44", NULL, NULL);
   if (window == NULL) {
      const char* description;
      glfwGetError(&description);
      throw std::runtime_error("GLFW Window Creation Failed: " + std::string(description ? description : "Unknown error"));
   }
   glfwMakeContextCurrent(window);

   gladLoadGL();

   // Query the actual framebuffer size and set the viewport accordingly
   glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
   renderWidth = frameBufferWidth;
   renderHeight = frameBufferHeight;
   glViewport(0, 0, renderWidth, renderHeight);
   glfwSetWindowUserPointer(window, &context);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
   // Calculate the ImGui panel width as a percentage of the new framebuffer width
   CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
   if (ctx->window) {
      ctx->window->frameBufferWidth = width;
      ctx->window->frameBufferHeight = height;
      ctx->window->renderWidth = width;
      ctx->window->renderHeight = height;
      glViewport(0, 0, ctx->window->renderWidth, ctx->window->renderHeight);
   }
}

void Window::setViewport() {
   glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
   renderWidth = frameBufferWidth;
   renderHeight = frameBufferHeight;
   glViewport(0, 0, renderWidth, renderHeight);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}
bool Window::isActive() {
   return !glfwWindowShouldClose(window);
}
void Window::SwapBuffers() {
   glfwSwapBuffers(window);
   glfwPollEvents();
}

void Window::Delete() {
   glfwDestroyWindow(window);
}