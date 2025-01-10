#ifndef INC_3DCAMERA_HPP
#define INC_3DCAMERA_HPP

#include <Camera.hpp>

class Camera3D final : public Camera {
   public:
   float yaw = 0.0f;
   float pitch = 0.0f;
   double lastMouseX = 0.0f;
   double lastMouseY = 0.0f;
   Camera3D(float* width, float* height, glm::vec3 position) : Camera(width, height, position) {};

   void Inputs(GLFWwindow* window) override;
   void updateMatrix(float FOVdeg, float nearPlane, float farPlane) override;
};

#endif //INC_3DCAMERA_HPP
