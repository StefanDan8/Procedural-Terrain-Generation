#define GLFW_INCLUDE_NONE

#include "gui/GUI.hpp"
#include <charconv>
#include <chrono>

#include "Camera2D.hpp"
#include "Camera3D.hpp"
#include "Fuse.hpp"
#include "PerlinNoise.hpp"
#include "Renderer.hpp"
#include "Terrain.hpp"
#include "Terrain3D.hpp"
#include "UserInterface.hpp"

#include "ShaderManager.hpp"

bool is3DMode = false;
bool switchedRecently = false;

std::vector<std::vector<std::string>> shaders = {
   {"Beach2DShader.vert", "Mono2DShader.vert"}, //2D shaders
   {"beachShader.vert", "default.vert", "Fjord.vert"} // 3D shaders
};

std::string previousVertexShader = shaders[is3DMode][0];
std::string previousFragmentShader = "default.frag";

std::string currentVertexShader = previousVertexShader;
std::string currentFragmentShader = previousFragmentShader;

const unsigned WINDOW_WIDTH = 1000;
const unsigned WINDOW_HEIGHT = 750;

// Both
int previousSeed = 42;
int seed = previousSeed;
double flattenFactor = 2.0;
double lastFlattenFactor = flattenFactor;

float oceanUpperBound = 0.01f;
float sandLowerBound = 0.01f;

// 3D Specific
float sandUpperBound = 0.04f;
float grassLowerBound = 0.04f;
float grassUpperBound = 0.1f;

// 2D Specific
float lowerGrassUpperBound;
float upperGrassUpperBound;
float lowerPeaksBound;
float upperPeaksBound;

// int frameSinceChange = -1;
// const int fuse = 30; // 30 frames until change takes placec
int fpsPrintTimer = 0;
float printFps = 0.0f;
float fpsAvg = 0.0f;

/**
 * Renders the common ImGui elements for both 2D and 3D modes.
 * @author SD
 */
void RenderCommonImGui() {
   ImGui::Text("Pick display mode:");

   if (ImGui::Button("2D Mode")) {
      is3DMode = false;
      switchedRecently = true;
   }
   ImGui::SameLine();
   if (ImGui::Button("3D Mode")) {
      is3DMode = true;
      switchedRecently = true;
   }
}

/**
 * Renders the shader dropdown menu in the ImGui window.
 * @author SD, PK (minor modifications)
 */
void shaderDropdown() {
   ImGui::PushItemWidth(140.f);
   static unsigned currentItem = 0; // Index of the currently selected item
   //std::vector<std::string> items = {"Option 1", "Option 2", "Option 3", "Option 4"};

   ImGui::Text("\nSelect a Shader");
   // is3DMode is a boolean, which really just means 0 for 2D, 1 for 3D
   if (ImGui::BeginCombo("##xa", shaders[is3DMode][currentItem].c_str())) {
      for (unsigned i = 0; i < shaders[is3DMode].size(); i++) {
         bool isSelected = (currentItem == i);
         if (ImGui::Selectable(shaders[is3DMode][i].c_str(), isSelected)) {
            currentItem = i; // Update selected item index
            currentVertexShader = shaders[is3DMode][i];
         }
         // Set the initial focus when opening the combo (scrolling to current item)
         if (isSelected) {
            ImGui::SetItemDefaultFocus();
         }
      }
      ImGui::EndCombo();
   }
}

void _3DInputControls() {
   ImGui::Text(R"(
--- Keyboard Controls ---
NOTE: cursor must be in the rendering area

  W  = Zoom In
  S  = Zoom Out
  A  = Move Camera Left
  D  = Move Camera Right

  Rotate Object:

  Left Ctrl + W  = Increase Pitch
  Left Ctrl + A  = Decrease Yaw
  Left Ctrl + S  = Decrease Pitch
  Left Ctrl + D  = Increase Yaw

--- Mouse Controls ---

  Click and Drag to move camera

)");
}

void _2DInputControls() {
   ImGui::Text(R"(
--- Keyboard Controls ---
NOTE: Cursor must be in the rendering area

   W = Move Camera Up
   S = Move Camera Down
   A = Move Camera Left
   D = Move Camera Right

--- Mouse Controls ---

   Click and Drag to move camera
   Scroll Wheel = Zoom In/Out

)");
}

/**
 * Renders the save to file options for 3D mode in the ImGui window. (obj)
 * @param mesh Mesh to be saved to a file
 * @author SD
 */
void saveToFile3D(Mesh& mesh) {
   ImGui::Text("\n\nSave Current Object to .obj\n");
   static char _user_save_path[256] = "";
   ImGui::InputText("Filename", _user_save_path, sizeof(_user_save_path));
   if (ImGui::Button("Save")) {
      ExportToObj(mesh, std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path + ".obj");
   }
}

/**
 * Renders the save to file options for 2D mode in the ImGui window. (png and ppm)
 * @param map Map to be saved to a file
 * @author PK
 */
void saveToFile2D(Mesh& mesh) {
   ImGui::Text("\nSave Current Image");
   static char _user_save_path[256] = "";
   ImGui::InputText("Filename", _user_save_path, sizeof(_user_save_path));
   if (ImGui::Button("Save to .png")) {
      auto filename = std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path + ".png";
      mesh.exportToPNG(filename);
   }
   ImGui::SameLine();
   if (ImGui::Button("Save to .ppm")) {
      auto filename = std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path + ".ppm";
      mesh.exportToPPM(filename);
   }
}

/**
 * Renders the user shader parameters in the ImGui window.
 * @param manager ShaderManger object
 * @author SD, PK (extracted method)
 */
void userShaderParameters(ShaderManager& manager) {
   ImGui::Text("User Shader Parameters\n");
   unsigned num = manager.getShader().userFloatUniforms.size();
   for (unsigned i = 0; i < num; ++i) {
      ImGui::PushID(i * 999);
      ImGui::SetNextItemWidth(90.f);
      ImGui::SliderFloat("", &(manager.getShader().userFloatValues[i]), -1.0f, 1.0f);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(i * 998);
      if (ImGui::Button("-")) {
         manager.getShader().userFloatValues[i] -= 0.002f;
      }
      ImGui::SameLine();
      ImGui::PopID();
      ImGui::PushID(i * 777);
      if (ImGui::Button("+")) {
         manager.getShader().userFloatValues[i] += 0.002f;
      }
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::Text(manager.getShader().userFloatUniforms[i].c_str());
   }
}

/**
 * Renders the common mesh settings in the ImGui window.
 * @author SD, PK (extracted method)
 */
void meshSettings() {
   ImGui::Text("Mesh Settings");
   ImGui::SetNextItemWidth(80.f);
   ImGui::InputInt("Seed", &seed);

   ImGui::PushItemWidth(70.f);
   ImGui::InputDouble("##xx", &flattenFactor, 0.0, 0.0, "%.2f");
   ImGui::SameLine();
   if (ImGui::Button("-")) {
      flattenFactor -= 0.2;
   }
   ImGui::SameLine();
   if (ImGui::Button("+")) {
      flattenFactor += 0.2;
   }
   ImGui::SameLine();
   ImGui::Text("Flatten Factor");
}
bool InputUnsigned(const char* label, unsigned int* v, unsigned int step = 1, unsigned int step_fast = 10, ImGuiInputTextFlags flags = 0) {
   int value = static_cast<int>(*v); // Cast to signed for ImGui input
   bool changed = ImGui::InputInt(label, &value, static_cast<int>(step), static_cast<int>(step_fast), flags);

   // Enforce unsigned constraint
   if (value < 0) {
      value = 0; // Clamp to zero if negative
   }
   *v = static_cast<unsigned int>(value); // Update the original unsigned value
   return changed;
}
void noiseLayersGui(Terrain& terrain, Fuse& fuse) {
   ImGui::Text("Noise parameters");
   unsigned index = 0;
   ImGui::Text("Chunk Size          Weight");
   for (auto& layerParam : terrain.getNoiseParams()) {
      ImGui::PushID(index * 9999 + 76);
      if (InputUnsigned("##xx", &layerParam.first)) {
         fuse.planLayerUpdate(index, NOISE_LAYER, CHUNK_SIZE);
      }
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(index * 99999 + 77);
      if (ImGui::InputDouble("##xx", &layerParam.second)) {
         fuse.planLayerUpdate(index, NOISE_LAYER, WEIGHT);
      }
      ImGui::PopID();
      index++;
   }
   index = 0;
   ImGui::Text("Baseline noise parameters");
   for (auto& layerParam : terrain.getBaselineParams()) {
      ImGui::PushID(index * 9999 + 86);
      if (InputUnsigned("##xx", &layerParam.first)) {
         fuse.planLayerUpdate(index, BASELINE_LAYER, CHUNK_SIZE);
      }
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::PushID(index * 99999 + 87);
      if (ImGui::InputDouble("##xx", &layerParam.second)) {
         fuse.planLayerUpdate(index, BASELINE_LAYER, WEIGHT);
      }
      ImGui::PopID();
      index++;
   }
}

/**
 * Renders the full ImGui window for the 3D mode.
 * @param manager ShaderManager object, passed for userShaderParameters()
 * @param mesh Mesh object to be rendered, passed for saveToFile3D()
 * @author SD, PK (extracted method)
 */
void Render3DImGui(ShaderManager& manager, Terrain& terrain, Fuse& fuse, float fps) {
   fpsPrintTimer++;
   fpsAvg += fps;
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   RenderCommonImGui();
   ImGui::Text("\n3D Mode\n\n");

   userShaderParameters(manager);
   meshSettings();
   shaderDropdown();
   noiseLayersGui(terrain, fuse);
   saveToFile3D(terrain.getMesh());
   _3DInputControls();
   if (fpsPrintTimer > 99) {
      fpsPrintTimer = 0;
      printFps = fpsAvg / 100;
      fpsAvg = 0.0;
   }
   ImGui::Text("FPS: %1.f", printFps);
   ImGui::End();
}

/**
 * Renders the full ImGui window for the 2D mode.
 * @param manager ShaderManager object, passed for userShaderParameters()
 * @param map Map object to be rendered, passed for saveToFile2D()
 * @author PK, SD
 */
void Render2DImGui(ShaderManager& manager, Terrain& terrain, float fps) {
   fpsPrintTimer++;
   fpsAvg += fps;
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   RenderCommonImGui();
   ImGui::Text("\n2D Mode\n\n");

   userShaderParameters(manager);
   meshSettings();

   shaderDropdown();

   saveToFile2D(terrain.getMesh());
   _2DInputControls();
   if (fpsPrintTimer > 99) {
      fpsPrintTimer = 0;
      printFps = fpsAvg / 100;
      fpsAvg = 0.0;
   }
   ImGui::Text("FPS: %1.f", printFps);
   ImGui::End();
}

int main() {
   auto lastFrameTime = std::chrono::high_resolution_clock::now();
   perlin::AppConfig::initialize(42);

   Window window(WINDOW_WIDTH, WINDOW_HEIGHT);
   GUI gui(window);

   ShaderManager shaderManager(previousVertexShader, previousFragmentShader);

   BasicConfigParams configParams{42, 1440, 1440, 2.0};
   std::vector<std::pair<unsigned, double>> paramsNoise{std::make_pair(720, 30), std::make_pair(360, 250), std::make_pair(180, 50),
                                                        std::make_pair(90, 50), std::make_pair(45, 20), std::make_pair(12, 5), std::make_pair(8, 2), std::make_pair(3, 1)};
   std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 2), std::make_pair(120, 2), std::make_pair(60, 2), std::make_pair(30, 1)};

   Terrain terrain(configParams, paramsNoise, filterParams);

   Fuse fuse(200, paramsNoise.size(), filterParams.size()); // create fuse with 200 frames capacity

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

      if (is3DMode) {
         Render3DImGui(shaderManager, terrain, fuse, 1.0f / elapsedSinceLastFrame);
      } else {
         Render2DImGui(shaderManager, terrain, 1.0f / elapsedSinceLastFrame);
      }

      glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (switchedRecently) {
         switchedRecently = false;
         currentVertexShader = previousVertexShader = shaders[is3DMode][0];
         shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
      }

      if (currentVertexShader.compare(previousVertexShader)) {
         previousVertexShader = currentVertexShader;
         std::cout << "Changed shader\n";
         shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
      }

      shaderManager.getShader().setUniforms();

      // // Render ImGUI
      ImGui::Render();

      fuse.tick();

      if (previousSeed != seed) {
         fuse.planSeedUpdate();
         previousSeed = seed;
      }
      if (abs(flattenFactor - lastFlattenFactor) > 0.1) { // only do meaningful changes
         fuse.planFlattenFactorUpdate();
         lastFlattenFactor = flattenFactor;
      }
      if (fuse.isSeedUpdateNow()) {
         terrain.createFromSeed(seed);
      }
      if (fuse.isFlattenFactorUpdateNow()) {
         terrain.computeMesh(flattenFactor);
      }

      if (fuse.isLayerUpdateNow()) {
         terrain.recomputeLayers(fuse.noiseLayerUpdate, fuse.baselineLayerUpdate);
      }

      Camera* camera = is3DMode ? static_cast<Camera*>(&camera_3d) : static_cast<Camera*>(&camera_2d);

      camera->Inputs(window.getWindow(), elapsedSinceLastFrame);
      camera->updateMatrix(45.0f, 0.1f, 100.0f);

      terrain.Draw(shaderManager.getShader(), *camera);
      gui.RenderDrawData();
   }

   shaderManager.Delete();
   window.Delete();
   gui.Shutdown();
   glfwTerminate();
   return 0;
}
