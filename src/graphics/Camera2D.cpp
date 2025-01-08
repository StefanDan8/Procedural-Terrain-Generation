#include "Camera2D.hpp"

void Camera2D::Inputs(GLFWwindow* window) {
   // Stores the coordinates of the cursor
   double mouseX;
   double mouseY;

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

   glfwSetScrollCallback(window, ScrollCallback);
}

void Camera2D::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
   Camera2D* camera = static_cast<Camera2D*>(glfwGetWindowUserPointer(window));
   if (camera == nullptr) return;

   // Consider clamping function here
   camera->Position += camera->speed * glm::vec3(0.0f, 0.0f, -yoffset);
}