#ifndef INC_3DCAMERA_HPP
#define INC_3DCAMERA_HPP

#include <Camera.hpp>

/**
 * Class for managing a 3D camera
 * @author SD, PK
 */
class Camera3D final : public Camera {
   public:
   float yaw = 0.0f;
   float pitch = 20.0f;
   double lastMouseX = 0.0f;
   double lastMouseY = 0.0f;
   Camera3D(Window& window, glm::vec3 position) : Camera(window, position) {};

   /// @brief updates the `cameraMatrix` based on current values of `Position` and `Orientation`
   /// @param FOVdeg field of view in degrees
   /// @param nearPlane minimum distance at which the camera can see
   /// @param farPlane maximum distance at which the camera can see
   /// @note additionally, it handles object rotation through `yaw` and `pitch`
   void updateMatrix(float FOVdeg, float nearPlane, float farPlane) override;

   void Inputs(float elapsedTimeSinceLastFrame) override;

   private:
   /// @brief handles keyboard and mouse events
   /// @param window
   /// @note additionally, it handles object rotation through `yaw` and `pitch`
   void _Inputs(GLFWwindow* window, float elapsedTimeSinceLastFrame) override;
};

#endif //INC_3DCAMERA_HPP
