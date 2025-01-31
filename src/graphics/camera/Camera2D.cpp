#include "Camera2D.hpp"

void Camera2D::Inputs(GLFWwindow* window, float elapsedTimeSinceLastFrame) {
   // Stores the coordinates of the cursor
   double mouseX;
   double mouseY;
   // Get the cursor position variables
   glfwGetCursorPos(window, &mouseX, &mouseY);

   // Calculate the speed, considering the z component of the camera position. Meaning, when zoomed in, the camera moves slower
   const float contextualSpeed = elapsedTimeSinceLastFrame * speed * (Position.z - .5f);

   // Keyboard related operations, only apply when the cursor is not in the rendering area
   if (mouseX > *width / 3) {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
         Position += contextualSpeed * -glm::normalize(glm::cross(Orientation, Right));
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
         Position += contextualSpeed * -glm::normalize(glm::cross(Orientation, Up));
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
         Position += contextualSpeed * glm::normalize(glm::cross(Orientation, Right));
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
         Position += contextualSpeed * glm::normalize(glm::cross(Orientation, Up));
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
      // Change position. The speed is multiplied by the z component of the camera position, meaning when zoomed in, the camera moves slower
      Position += contextualSpeed * glm::vec3(-deltaX, deltaY, 0.0f);
   } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      firstClick = true;
   }
}

void Camera2D::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
   auto camera = static_cast<Camera2D*>(glfwGetWindowUserPointer(window));
   if (camera == nullptr) return;

   auto newPos = camera->Position + camera->speed * 0.01f * camera->mouseScrollMultiplier * glm::vec3(0.0f, 0.0f, -yOffset);
   newPos.z = std::clamp(newPos.z, 1.1f, 2.5f); // Prevents zooming too far in or out

   camera->Position = newPos;
}