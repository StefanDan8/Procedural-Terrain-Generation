#define GLFW_INCLUDE_NONE

#include "GUI.hpp"

#include "Camera2D.hpp"
#include "Camera3D.hpp"

const unsigned WINDOW_WIDTH = 1200;
const unsigned WINDOW_HEIGHT = 800;

int main() {
   auto lastFrameTime = std::chrono::steady_clock::now();
   perlin::AppConfig::initialize(42);

   Window window(WINDOW_WIDTH, WINDOW_HEIGHT);
   GUI gui(window);

   BasicConfigParams configParams{42, 1440, 1440, 2.0};
   std::vector<std::pair<unsigned, double>> paramsNoise{std::make_pair(720, 30), std::make_pair(360, 250), std::make_pair(180, 50),
                                                        std::make_pair(90, 50), std::make_pair(45, 20), std::make_pair(12, 5), std::make_pair(8, 2), std::make_pair(3, 1)};
   std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 2), std::make_pair(120, 2), std::make_pair(60, 2), std::make_pair(30, 1)};

   Terrain terrain(configParams, paramsNoise, filterParams);

   glEnable(GL_DEPTH_TEST);

   // Create both here else it'll recreate the camera every frame
   Camera3D camera_3d(window, glm::vec3(-0.15f, 0.0f, 1.6f));
   Camera2D camera_2d(window, glm::vec3(-0.3f, 0.0f, 2.5f)); // handpicked to fit nicely
   window.context.window = &window;
   window.context.camera2D = &camera_2d;
   while (window.isActive()) {
      auto currentTime = std::chrono::steady_clock::now();
      std::chrono::duration<float> deltaTime = currentTime - lastFrameTime;
      lastFrameTime = currentTime;

      float elapsedSinceLastFrame = deltaTime.count(); // in seconds
      window.setViewport();
      gui.NewFrame();

      gui.DisplayGUI(terrain, elapsedSinceLastFrame);
      Camera* camera = nullptr;
      if (gui.is3DModeActive()) {
         window.context.camera2D = nullptr;
         camera = static_cast<Camera*>(&camera_3d);
      } else {
         window.context.camera2D = &camera_2d;
         camera = static_cast<Camera*>(&camera_2d);
      }
      if (!gui.isGUIHovered()) {
         camera->Inputs(elapsedSinceLastFrame);
      }
      camera->updateMatrix(45.0f, 0.1f, 100.0f);

      gui.DrawTerrain(terrain, *camera);

      gui.RenderDrawData();
   }

   gui.DeleteShaderManager();
   window.Delete();
   gui.Shutdown();
   glfwTerminate();
   return 0;
}
