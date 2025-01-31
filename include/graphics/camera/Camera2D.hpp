#ifndef INC_2DCAMERA_HPP
#define INC_2DCAMERA_HPP

#include <Camera.hpp>

/**
 * Class for managing a 2D camera
 * @author PK
 */
class Camera2D final : public Camera {
   public:
   // To calculate deltas
   double lastMouseX = 0.0f, lastMouseY = 0.0f;
   // Increase speed on mouse drag + scroll
   float mouseScrollMultiplier = 3.0f;

   Camera2D(float* width, float* height, glm::vec3 position, GLFWwindow* window) : Camera(width, height, position) {
      glfwSetWindowUserPointer(window, this);
   };

   /**
    * Handles keyboard and mouse events for moving the 2D camera
    * @param window Pointer of the current window (in which the camera is in)
    * @author PK
    */
   void Inputs(GLFWwindow* window, float elapsedTimeSinceLastFrame) override;

   /**
    * Static function to use in GLFW callback for handling scroll events.
    * @param window Pointer of the current window (in which the camera is in)
    * @param xOffset Value of the horizontal scroll. Provided by GLFW callback
    * @param yOffset Value of the vertical scroll. Provided by GLFW callback
    * @author PK
    */
   static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};

#endif //INC_2DCAMERA_HPP
