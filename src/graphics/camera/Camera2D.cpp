#include "Camera2D.hpp"

void Camera2D::Inputs(float elapsedTimeSinceLastFrame) {
   _Inputs(window.getWindow(), elapsedTimeSinceLastFrame);
}

void Camera2D::_Inputs(GLFWwindow* glfwWindow, float elapsedTimeSinceLastFrame) {
   // Stores the coordinates of the cursor
   double mouseX;
   double mouseY;
   // Get the cursor position variables
   glfwGetCursorPos(glfwWindow, &mouseX, &mouseY);

   // Calculate the speed, considering the z component of the camera position. Meaning, when zoomed in, the camera moves slower
   const float contextualSpeed = elapsedTimeSinceLastFrame * speed * (Position.z - .5f);

   if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
      Position += contextualSpeed * -glm::normalize(glm::cross(Orientation, Right));
   }
   if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
      Position += contextualSpeed * -glm::normalize(glm::cross(Orientation, Up));
   }
   if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
      Position += contextualSpeed * glm::normalize(glm::cross(Orientation, Right));
   }
   if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
      Position += contextualSpeed * glm::normalize(glm::cross(Orientation, Up));
   }
   glfwSetScrollCallback(glfwWindow, ScrollCallback);

   // Mouse related operations
   if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      if (firstClick) {
         // Prevents camera from jumping on the first click
         glfwGetCursorPos(glfwWindow, &lastMouseX, &lastMouseY);
         firstClick = false;
      }

      // GLFW_CURSOR_DISABLED hides the cursor and allows for unlimited movement
      glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      if (glfwRawMouseMotionSupported()) // If supported, use raw mouse motion, disabling acceleration
         glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

      // Update the cursor position variables
      glfwGetCursorPos(glfwWindow, &mouseX, &mouseY);
      // Calculate delta
      const double deltaX = (mouseX - lastMouseX) / 50;
      const double deltaY = (mouseY - lastMouseY) / 50;
      // Update last position
      lastMouseX = mouseX;
      lastMouseY = mouseY;
      // Change position. The speed is multiplied by the z component of the camera position, meaning when zoomed in, the camera moves slower
      Position += contextualSpeed * glm::vec3(-deltaX, deltaY, 0.0f);
   } else if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
      glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      firstClick = true;
   }
}

void Camera2D::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
   CallbackContext* ctx = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
   (void) xOffset; // to avoid warning;
   if (ctx->camera2D) {
      auto newPos = ctx->camera2D->Position + ctx->camera2D->speed * 0.01f * ctx->camera2D->mouseScrollMultiplier * glm::vec3(0.0f, 0.0f, -yOffset);
      newPos.z = std::clamp(newPos.z, 1.1f, 2.5f);
      ctx->camera2D->Position = newPos;
   }
}