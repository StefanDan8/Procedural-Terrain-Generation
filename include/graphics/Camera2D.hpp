#ifndef INC_2DCAMERA_HPP
#define INC_2DCAMERA_HPP

#include <Camera.hpp>

class Camera2D final : public Camera {
   public:
   glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f);

   double lastMouseX = 0.0f;
   double lastMouseY = 0.0f;

   Camera2D(float *width, float *height, glm::vec3 position, GLFWwindow* window): Camera(width, height, position) {
      glfwSetWindowUserPointer(window, this);
   };

   void Inputs(GLFWwindow* window) override;
   // This function is static because it is a callback, and will not work otherwise.
   static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};

#endif //INC_2DCAMERA_HPP
