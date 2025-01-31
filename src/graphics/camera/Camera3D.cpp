#include "Camera3D.hpp"

void Camera3D::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
   glm::mat4 view = glm::mat4(1.0f);
   glm::mat4 projection = glm::mat4(1.0f);
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::rotate(model, glm::radians(pitch), Right);
   model = glm::rotate(model, glm::radians(yaw), Up);
   view = glm::lookAt(Position, Position + Orientation, Up);
   projection = glm::perspective(glm::radians(FOVdeg), (float) (*width / *height), nearPlane, farPlane);
   cameraMatrix = projection * view * model;
}

void Camera3D::Inputs(GLFWwindow* window, float elapsedTimeSinceLastFrame) {
   // Stores the coordinates of the cursor
   double mouseX;
   double mouseY;
   // Fetches the coordinates of the cursor
   glfwGetCursorPos(window, &mouseX, &mouseY);

   // If mouse is in the area of user interface, then the Camera should not respond
   // in order to allow, for example, using W,A,S,D in text fields
   if (mouseX < *width / 3) {
      return;
   }
   /*
   --- Keyboard controls ---

   W = Zoom In
   S = Zoom Out
   A = move camera left
   D = move camera right
   Rotate Object
   Left Ctrl+W = increase pitch 
   Left Ctrl+A = decrease yaw
   Left Ctrl+S = decrease pitch
   Left Ctrl+D = increase yaw
   */
   if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS) {
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
         Position += elapsedTimeSinceLastFrame * speed * Orientation;
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
         Position += elapsedTimeSinceLastFrame * speed * -Right;
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
         Position += elapsedTimeSinceLastFrame * speed * -Orientation;
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
         Position += elapsedTimeSinceLastFrame * speed * Right;
      }
   } else { // Left Ctrl pressed
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
         pitch += elapsedTimeSinceLastFrame * sensitivity;
      }
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
         pitch += -elapsedTimeSinceLastFrame * sensitivity;
      }
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
         yaw += -elapsedTimeSinceLastFrame * sensitivity;
      }
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
         yaw += elapsedTimeSinceLastFrame * sensitivity;
      }
   }

   /*
   --- Mouse controls ---
   Click and drag to move
   */

   // Mouse related operations
   if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      // GLFW_CURSOR_DISABLED hides the cursor and allows for unlimited movement
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

      if (glfwRawMouseMotionSupported()) // If supported, use raw mouse motion, disabling acceleration
         glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

      if (firstClick) {
         // Prevents camera from jumping on the first click
         glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
         firstClick = false;
      }

      // Update the cursor position variables
      glfwGetCursorPos(window, &mouseX, &mouseY);
      // Calculate delta
      const double deltaX = (mouseX - lastMouseX) / 50;
      const double deltaY = (mouseY - lastMouseY) / 50;
      // Update last position
      lastMouseX = mouseX;
      lastMouseY = mouseY;
      // Change position
      Position += elapsedTimeSinceLastFrame * speed * glm::vec3(-deltaX, deltaY, 0.0f);
   } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      firstClick = true;
   }
}
