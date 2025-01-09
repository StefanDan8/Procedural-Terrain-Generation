#ifndef INC_3DCAMERA_HPP
#define INC_3DCAMERA_HPP

#include <Camera.hpp>

class Camera3D final : public Camera {
   public:
   Camera3D(int width, int height, glm::vec3 position): Camera(width, height, position) {};

   void Inputs(GLFWwindow* window) override;
};

#endif //INC_3DCAMERA_HPP
