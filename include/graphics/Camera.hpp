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

/// @brief Class for managing the camera (point of view) of the rendering
class Camera {
   public:
   float* width;
   float* height;

   /// @brief position of the camera
   glm::vec3 Position;

   /// @brief the direction in which camera is pointing towards.
   /// @note positive Z-axis points towards the viewer
   glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);

   glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f);

   /// @brief matrix which describes the transformation applied to the objects being rendered
   glm::mat4 cameraMatrix = glm::mat4(1.0f);

   /// @brief scaling factor for keyboard input
   float speed = 0.01f;

   /// @brief scaling factor for mouse input
   float sensitivity = 0.1f;

   bool firstClick = true;

   Camera(float* width, float* height, glm::vec3 position) : width(width), height(height), Position(position) {}

   /// @brief passes `cameraMatrix` as uniform to a vertex shader
   /// @param shader vertex shader
   /// @param uniform name of cameraMatrix inside the shader
   void Matrix(Shader& shader, const char* uniform);

   /// @brief updates the `cameraMatrix` based on current values of `Position` and `Orientation`
   /// @param FOVdeg field of view in degrees
   /// @param nearPlane minimum distance at which the camera can see
   /// @param farPlane maximum distance at which the camera can see
   virtual void updateMatrix(float FOVdeg, float nearPlane, float farPlane);

   /// @brief handles keyboard and mouse events
   /// @param window
   virtual void Inputs(GLFWwindow* window) = 0;
};

#endif