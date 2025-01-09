#ifndef CAMERA_CLASS_HPP
#define CAMERA_CLASS_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include "ShaderClass.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <glad/glad.h>

#include "glfw-3.4/include/GLFW/glfw3.h"

class Camera {
public:
   int width;
   int height;

   glm::vec3 Position;
   glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
   glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::mat4 cameraMatrix = glm::mat4(1.0f);

   float speed = 0.01f;
   float sensitivity = 0.1f;

   bool firstClick = true;

   Camera(int width, int height, glm::vec3 position) : width(width), height(height), Position(position) {}

   void Matrix(Shader& shader, const char* uniform);
   void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
   virtual void Inputs(GLFWwindow* window) = 0;
};

#endif