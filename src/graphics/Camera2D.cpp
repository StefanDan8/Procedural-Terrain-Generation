#include "Camera2D.hpp"

void Camera2D::Inputs(GLFWwindow* window) {
   // Stores the coordinates of the cursor
   double mouseX;
   double mouseY;
   // Get the cursor position variables
   glfwGetCursorPos(window, &mouseX, &mouseY);

   // Keyboard related operations, only apply when the cursor is not in the rendering area
   if (mouseX > *width / 3) {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
         Position += speed * -glm::normalize(glm::cross(Orientation, Right));
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
         Position += speed * -glm::normalize(glm::cross(Orientation, Up));
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
         Position += speed * glm::normalize(glm::cross(Orientation, Right));
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
         Position += speed * glm::normalize(glm::cross(Orientation, Up));
      }
   }

   glfwSetScrollCallback(window, ScrollCallback);

   // Mouse related operations
   if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      if (firstClick) {
         // Check the current mouse position is in the rendering area. If not, return.
         if (mouseX < *width / 3) return;

         // Prevents camera from jumping on the first click
         glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
         firstClick = false;
      }

      // GLFW_CURSOR_DISABLED hides the cursor and allows for unlimited movement
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      if (glfwRawMouseMotionSupported()) // If supported, use raw mouse motion, disabling acceleration
         glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

      // Update the cursor position variables
      glfwGetCursorPos(window, &mouseX, &mouseY);
      // Calculate delta
      const double deltaX = (mouseX - lastMouseX) / 50;
      const double deltaY = (mouseY - lastMouseY) / 50;
      // Update last position
      lastMouseX = mouseX;
      lastMouseY = mouseY;
      // Change position
      Position += speed * glm::vec3(-deltaX, deltaY, 0.0f);
   } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      firstClick = true;
   }
}

void Camera2D::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
   auto camera = static_cast<Camera2D*>(glfwGetWindowUserPointer(window));
   if (camera == nullptr) return;

   // Consider clamping function here
   camera->Position += camera->speed * glm::vec3(0.0f, 0.0f, -yOffset);
}