#define GLFW_INCLUDE_NONE

#include "GUI.hpp"

#include <chrono>

#include "Camera2D.hpp"
#include "Camera3D.hpp"

const unsigned WINDOW_WIDTH = 1200;
const unsigned WINDOW_HEIGHT = 800;

int main() {
   auto lastFrameTime = std::chrono::high_resolution_clock::now();
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
   Camera3D camera_3d(window.getRenderWidth(), window.getRenderHeight(), glm::vec3(-0.1f, 0.0f, 2.0f));
   Camera2D camera_2d(window.getRenderWidth(), window.getRenderHeight(), glm::vec3(-0.1f, 0.0f, 2.5f), window.getWindow()); // handpicked to fit nicely
   while (window.isActive()) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      std::chrono::duration<float> deltaTime = currentTime - lastFrameTime;
      lastFrameTime = currentTime;

      float elapsedSinceLastFrame = deltaTime.count(); // in seconds
      window.setViewport();
      gui.NewFrame();
      gui.DisplayGUI(terrain, elapsedSinceLastFrame);

      Camera* camera = gui.is3DModeActive() ? static_cast<Camera*>(&camera_3d) : static_cast<Camera*>(&camera_2d);

      camera->Inputs(window.getWindow(), elapsedSinceLastFrame);
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
