#ifndef INC_2DCAMERA_HPP
#define INC_2DCAMERA_HPP

#include <Camera.hpp>

class Camera2D final : public Camera {
   public:
   glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f);
   Camera2D(int width, int height, glm::vec3 position, GLFWwindow* window): Camera(width, height, position) {
      glfwSetWindowUserPointer(window, this);
   };

   void Inputs(GLFWwindow* window) override;
   static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif //INC_2DCAMERA_HPP
