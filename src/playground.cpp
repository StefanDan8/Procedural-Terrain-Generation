#define GLFW_INCLUDE_NONE

// Include standard headers
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// Include source files
#include "AppConfig.hpp"
#include "Camera2D.hpp"
#include "Camera3D.hpp"
#include "Fuse.hpp"
#include "PerlinOOP.hpp"
#include "Renderer.hpp"
#include "Terrain.hpp"
#include "Terrain3D.hpp"
#include "UserInterface.hpp"

// Include additional graphics files
#include "Map.hpp"
#include "Mesh.hpp"
#include "ShaderManager.hpp"

bool is3DMode = false;
bool switchedRecently = false;

std::vector<std::vector<std::string>> shaders = {
   {"Beach2DShader.vert", "Mono2DShader.vert"}, //2D shaders
   {"beachShader.vert", "default.vert"} // 3D shaders
};

std::string previousVertexShader = shaders[is3DMode][0];
std::string previousFragmentShader = "default.frag";

std::string currentVertexShader = previousVertexShader;
std::string currentFragmentShader = previousFragmentShader;

int framebufferWidth, framebufferHeight;

const unsigned WINDOW_WIDTH = 1000;
const unsigned WINDOW_HEIGHT = 750;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
   // Calculate the ImGui panel width as a percentage of the new framebuffer width
   float IMGUI_WIDTH = 0.25f * width;
   float RENDER_WIDTH = width - IMGUI_WIDTH;

   // Set the OpenGL viewport for the rendering area
   glViewport((int) IMGUI_WIDTH, 0, (int) RENDER_WIDTH, height);

   // Update global framebuffer dimensions (if needed elsewhere)
   framebufferWidth = width;
   framebufferHeight = height;
}

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
void saveToFile2D(Map& map) {
   ImGui::Text("\nSave Current Image");
   static char _user_save_path[256] = "";
   ImGui::InputText("Filename", _user_save_path, sizeof(_user_save_path));
   if (ImGui::Button("Save to .png")) {
      auto filename = std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path + ".png";
      map.exportToPNG(filename);
   }
   ImGui::SameLine();
   if (ImGui::Button("Save to .ppm")) {
      auto filename = std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path + ".ppm";
      map.exportToPPM(filename);
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
   for (auto& layerParam : terrain.noiseParams) {
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
   for (auto& layerParam : terrain.baselineParams) {
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
// void Render3DImGui(ShaderManager& manager, Mesh& mesh, Terrain3D& terrain, float fps) {
//    fpsPrintTimer++;
//    fpsAvg += fps;
//    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
//    RenderCommonImGui();
//    ImGui::Text("\n3D Mode\n\n");

//    userShaderParameters(manager);
//    meshSettings();
//    shaderDropdown();
//    noiseLayersGui(terrain);
//    saveToFile3D(mesh);
//    _3DInputControls();
//    if (fpsPrintTimer > 99) {
//       fpsPrintTimer = 0;
//       printFps = fpsAvg / 100;
//       fpsAvg = 0.0;
//    }
//    ImGui::Text("FPS: %1.f", printFps);
//    ImGui::End();
// }
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
   saveToFile3D(terrain.mesh.value());
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
void Render2DImGui(ShaderManager& manager, Map& map, float fps) {
   fpsPrintTimer++;
   fpsAvg += fps;
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   RenderCommonImGui();
   ImGui::Text("\n2D Mode\n\n");

   userShaderParameters(manager);
   meshSettings();

   shaderDropdown();

   saveToFile2D(map);
   _2DInputControls();
   if (fpsPrintTimer > 99) {
      fpsPrintTimer = 0;
      printFps = fpsAvg / 100;
      fpsAvg = 0.0;
   }
   ImGui::Text("FPS: %1.f", printFps);
   ImGui::End();
}

/**
 * Generates noise for both 2D and 3D modes.
 * @param seed Seed for the noise generation
 * @param flatteningFactor Flattening factor for the noise generation
 * @return 2D vector of doubles representing the generated noise
 * @note Shouldn't be called directly, rather through generateMapFromSeed() and generateMeshFromSeed()
 * @author SD, PK (extracted method)
 */
std::vector<std::vector<double>> commonGeneration(const int seed, const double flatteningFactor) {
   perlin::AppConfig::getInstance().setGenerator(seed);

   const unsigned sizeX = 1440;
   const unsigned sizeY = 1440;

   // Parameters for the first noise layer
   std::vector<std::pair<unsigned, double>> params4{std::make_pair(720, 30), std::make_pair(360, 250), std::make_pair(180, 50),
                                                    std::make_pair(90, 50), std::make_pair(45, 20), std::make_pair(12, 5), std::make_pair(8, 2), std::make_pair(3, 1)};

   // Generate and fill the first noise layer
   perlin::PerlinNoise2D noise = perlin::PerlinNoise2D(sizeX, sizeY, params4);
   noise.fill();

   // Parameters for the second noise layer
   std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 2), std::make_pair(120, 2), std::make_pair(60, 2), std::make_pair(30, 1)};

   // Generate and fill the second noise layer
   perlin::PerlinNoise2D noiseFilter(sizeX, sizeY, filterParams);
   noiseFilter.fill();

   // Filter and normalize the noise
   noise.filterMatrix(noiseFilter);

   double sumWeight = noise.getWeightSum();
   perlin::matrix result(sizeX, std::vector<double>(sizeY, 0.0));
   result = noise.getResult();

   return render::normalizeUnit(result, sumWeight * flatteningFactor);
}

/**
 * Creates a Map object with noise (2D mode).
 * @param seed For the noise generation (random seed)
 * @param flatteningFactor For the noise generation (squishes the noise)
 * @return Map object with the generated noise
 * @author PK
 */
Map generateMapFromSeed(const int seed, const double flatteningFactor = 1.0f) {
   std::vector<std::vector<double>> normalized = commonGeneration(seed, flatteningFactor);
   Map myMap(normalized);
   return myMap;
}

/**
 * Creates a Mesh object with noise (3D mode).
 * @param seed For the noise generation (random seed)
 * @param flatteningFactor For the noise generation (squishes the noise)
 * @return Mesh object with the generated noise
 * @author SD
 */
Mesh generateMeshFromSeed(const int seed, const double flatteningFactor = 1.0) {
   std::vector<std::vector<double>> normalized = commonGeneration(seed, flatteningFactor);
   Mesh myMesh(normalized);
   return myMesh;
}

int main() {
   auto lastFrameTime = std::chrono::high_resolution_clock::now();
   perlin::AppConfig::initialize(42);

   glfwInit();

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Group44", NULL, NULL);
   if (window == NULL) {
      std::cout << "Failed to create GLFW window.\n";
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);

   gladLoadGL();

   // Query the actual framebuffer size and set the viewport accordingly
   glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
   float IMGUI_WIDTH = 0.25f * framebufferWidth;
   float RENDER_WIDTH = framebufferWidth - IMGUI_WIDTH;
   float RENDER_HEIGHT = framebufferHeight;

   glViewport((int) IMGUI_WIDTH, 0, (int) RENDER_WIDTH, framebufferHeight);

   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   // // ImGUI Setup
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls (optional)
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init("#version 330");
   ImGui::StyleColorsDark();

   ShaderManager shaderManager(previousVertexShader, previousFragmentShader);

   // Mesh myMesh = generateMeshFromSeed(42, flattenFactor);
   // Terrain3D terrain(1440, 1440, seed = 42, flattenFactor = 2.0);
   BasicConfigParams configParams{42, 1440, 1440, 2.0};
   std::vector<std::pair<unsigned, double>> paramsNoise{std::make_pair(720, 30), std::make_pair(360, 250), std::make_pair(180, 50),
                                                        std::make_pair(90, 50), std::make_pair(45, 20), std::make_pair(12, 5), std::make_pair(8, 2), std::make_pair(3, 1)};
   std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 2), std::make_pair(120, 2), std::make_pair(60, 2), std::make_pair(30, 1)};

   Terrain terrain(configParams, paramsNoise, filterParams);
   Map myMap = generateMapFromSeed(42, flattenFactor);

   Fuse fuse(200, paramsNoise.size(), filterParams.size()); // create fuse with 200 frames capacity

   glEnable(GL_DEPTH_TEST);

   // Create both here else it'll recreate the camera every frame
   Camera3D camera_3d(&RENDER_WIDTH, &RENDER_HEIGHT, glm::vec3(-0.1f, 0.0f, 2.0f));
   Camera2D camera_2d(&RENDER_WIDTH, &RENDER_HEIGHT, glm::vec3(-0.1f, 0.0f, 2.5f), window); // handpicked to fit nicely
   while (!glfwWindowShouldClose(window)) {
      auto currentTime = std::chrono::high_resolution_clock::now();
      std::chrono::duration<float> deltaTime = currentTime - lastFrameTime;
      lastFrameTime = currentTime;

      float elapsedSinceLastFrame = deltaTime.count(); // in seconds
      //std::cout<<elapsedSinceLastFrame<<std::endl;

      // Recalculate the framebuffer size and set the viewport accordingly
      glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
      IMGUI_WIDTH = 0.25f * framebufferWidth;
      RENDER_WIDTH = framebufferWidth - IMGUI_WIDTH;
      RENDER_HEIGHT = framebufferHeight;

      glViewport((int) IMGUI_WIDTH, 0, (int) RENDER_WIDTH, framebufferHeight);
      glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

      // Start ImGUI frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ImGui::SetNextWindowSize(ImVec2(IMGUI_WIDTH, framebufferHeight), ImGuiCond_Always);
      ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

      if (is3DMode) {
         Render3DImGui(shaderManager, terrain, fuse, 1.0f / elapsedSinceLastFrame);
         //Render3DImGui(shaderManager, myMesh, terrain, 1.0f / elapsedSinceLastFrame);
      } else {
         Render2DImGui(shaderManager, myMap, 1.0f / elapsedSinceLastFrame);
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

      // if (previousSeed != seed || abs(flattenFactor - lastFlattenFactor) > 0.1) { // only do meaningful changes
      //    frameSinceChange = 0;
      //    previousSeed = seed;
      //    lastFlattenFactor = flattenFactor;
      // } else {
      //    if (frameSinceChange > -1) {
      //       frameSinceChange++;
      //    }
      // }

      // if (frameSinceChange > fuse) {
      //    frameSinceChange = -1;
      //    std::cout << "recompute\n";
      //    //terrain.recompute(seed, flattenFactor);
      //    //myMesh = generateMeshFromSeed(seed, flattenFactor);
      // }

      Camera* camera = is3DMode ? static_cast<Camera*>(&camera_3d) : static_cast<Camera*>(&camera_2d);

      camera->Inputs(window, elapsedSinceLastFrame);
      camera->updateMatrix(45.0f, 0.1f, 100.0f);

      terrain.Draw(shaderManager.getShader(), *camera);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   shaderManager.Delete();

   glfwDestroyWindow(window);
   // Shutdown ImGUI
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
   glfwTerminate();
   return 0;
}
