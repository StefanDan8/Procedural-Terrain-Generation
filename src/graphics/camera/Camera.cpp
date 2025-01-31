#include "Camera.hpp"
#include "imgui/include/imgui.h"

void Camera::Matrix(Shader& shader, const char* uniform) {
   glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
   glm::mat4 view = glm::mat4(1.0f);
   glm::mat4 projection = glm::mat4(1.0f);
   view = glm::lookAt(Position, Position + Orientation, Up);
   projection = glm::perspective(glm::radians(FOVdeg), (float) (*window.getRenderWidth() / *window.getRenderHeight()), nearPlane, farPlane);
   cameraMatrix = projection * view;
}
