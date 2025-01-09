#define GLFW_INCLUDE_NONE

// Include standard headers
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

// Include source files
#include "AppConfig.hpp"
#include "Camera2D.hpp"
#include "PerlinOOP.hpp"
#include "Renderer.hpp"
#include "UserInterface.hpp"

// Include additional graphics files
#include "Mesh.hpp"
#include "ShaderManager.hpp"

#include <Camera3D.hpp>

bool is3Dmode = false;
bool switchedRecently = false;

std::vector<std::vector<std::string>> shaders = {
   {"2DShader.vert"}, //2D shaders
   {"beachShader.vert", "default.vert"} // 3D shaders
};

std::string previousVertexShader = shaders[is3Dmode][0];
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

// 3D Specific
float oceanUpperBound = 0.01f;
float sandLowerBound = 0.01f;
float sandUpperBound = 0.04f;
float grassLowerBound = 0.04f;
float grassUpperBound = 0.1f;

// 2D Specific
int chunkSize = 16;
int nChunksX = 16;
int nChunksY = 16;

// General
int previousSeed = 42;
int seed = previousSeed;

void RenderCommonImGui() {
   ImGui::Text("Pick display mode:");

   if (ImGui::Button("2D Mode")) {
      is3Dmode = false;
      switchedRecently = true;
   }
   ImGui::SameLine();
   if (ImGui::Button("3D Mode")) {
      is3Dmode = true;
      switchedRecently = true;
   }
}
void shaderDropdown() {
   static unsigned currentItem = 0; // Index of the currently selected item
   //std::vector<std::string> items = {"Option 1", "Option 2", "Option 3", "Option 4"};

   if (ImGui::BeginCombo("Select a Shader", shaders[is3Dmode][currentItem].c_str())) {
      for (unsigned i = 0; i < shaders[is3Dmode].size(); i++) {
         bool isSelected = (currentItem == i);
         if (ImGui::Selectable(shaders[is3Dmode][i].c_str(), isSelected)) {
            currentItem = i; // Update selected item index
            currentVertexShader = shaders[is3Dmode][i];
         }
         // Set the initial focus when opening the combo (scrolling to current item)
         if (isSelected) {
            ImGui::SetItemDefaultFocus();
         }
      }
      ImGui::EndCombo();
   }
}
void saveToFile() {
   ImGui::Text("\n\nSave current object\n\n");
   static char _user_save_path[256] = "";
   ImGui::InputText("Name of file", _user_save_path, sizeof(_user_save_path));
   if (ImGui::Button("Save current")) {
      std::cout << "Saved to " << _user_save_path << ".\n";
   }
}

void Render3DImGui(ShaderManager& manager) {
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   RenderCommonImGui();
   ImGui::Text("\n3D Mode\n\n");

   ImGui::Text("User shader parameters\n\n");
   unsigned num = manager.getShader().userFloatUniforms.size();
   for (unsigned i = 0; i < num; ++i) {
      ImGui::SliderFloat(manager.getShader().userFloatUniforms[i].c_str(), &(manager.getShader().userFloatValues[i]), -1.0f, 1.0f);
   }

   // ImGui::SliderFloat("Ocean Upper Bound", &oceanUpperBound, -1.0f, 0.5f);
   // ImGui::SliderFloat("Sand Lower Bound", &sandLowerBound, -0.1f, 0.5f);
   // ImGui::SliderFloat("Sand Upper Bound", &sandUpperBound, -0.1f, 0.5f);
   // ImGui::SliderFloat("Grass Lower Bound", &grassLowerBound, -0.1f, 0.5f);
   // ImGui::SliderFloat("Grass Upper Bound", &grassUpperBound, -0.1f, 0.5f);
   ImGui::Text("Mesh Settings");
   ImGui::InputInt("Seed", &seed);
   shaderDropdown();
   saveToFile();
   ImGui::End();
}

void Render2DImGui() {
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   RenderCommonImGui();
   ImGui::Text("\n2D Mode\n\n");
   ImGui::Text("Mesh Settings");
   ImGui::InputInt("Seed", &seed);

   ImGui::SliderInt("Chunk Size", &chunkSize, 0, 32);
   ImGui::SliderInt("Number of Chunks (X axis)", &nChunksX, 0, 32);
   ImGui::SliderInt("Number of Chunks (Y axis)", &nChunksY, 0, 32);
   shaderDropdown();
   saveToFile();
   ImGui::End();
}

Mesh generateMeshFromSeed(int seed) {
   perlin::AppConfig::getInstance().setGenerator(seed);

   const unsigned sizeX = 720;
   const unsigned sizeY = 720;
   std::vector<std::pair<unsigned, double>> params4{std::make_pair(720, 60), std::make_pair(360, 500), std::make_pair(180, 100),
                                                    std::make_pair(90, 100), std::make_pair(45, 20), std::make_pair(12, 10), std::make_pair(8, 5), std::make_pair(3, 2)};

   perlin::PerlinNoise2D noise = perlin::PerlinNoise2D(sizeX, sizeY, params4);
   perlin::matrix result(sizeX, std::vector<double>(sizeY, 0.0));
   noise.fill();
   result = noise.getResult();
   
   perlin::matrix filter(sizeX, std::vector<double>(sizeY, 0.0));
   std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 5), std::make_pair(120, 5), std::make_pair(60, 10), std::make_pair(30, 1)};
   perlin::PerlinNoise2D noiseFilter(sizeX, sizeY, filterParams);
   noiseFilter.fill();
   filter = noiseFilter.getResult();

   render::Max(result, filter);

   auto normalized = render::normalizeUnit(result);
   Mesh myMesh(normalized);
   return myMesh;
}

int main() {
   perlin::AppConfig::initialize(55);

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

   // //Shader shaderProgram("beachShader.vert", "default.frag");
   // Shader shaderProgram("2DShader.vert", "default.frag");
   // shaderProgram.Activate();
   ShaderManager shaderManager(previousVertexShader, previousFragmentShader);

   // Mesh myMesh(normalized);
   Mesh myMesh = generateMeshFromSeed(42);
   //ComputeNormals(myMesh);
   //PrintMesh(myMesh);

   glEnable(GL_DEPTH_TEST);

   // Create both here else it'll recreate the camera every frame
   Camera3D camera_3d( &RENDER_WIDTH, &RENDER_HEIGHT, glm::vec3(0.0f, 0.5f, 2.0f));
   Camera2D camera_2d( &RENDER_WIDTH, &RENDER_HEIGHT, glm::vec3(0.0f, 0.5f, 2.0f), window);

   while (!glfwWindowShouldClose(window)) {
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

      if (is3Dmode) {
         Render3DImGui(shaderManager);
      } else {
         Render2DImGui();
      }

      glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (switchedRecently) {
         switchedRecently = false;
         currentVertexShader = previousVertexShader = shaders[is3Dmode][0];
         shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
      }

      if (currentVertexShader.compare(previousVertexShader)) {
         previousVertexShader = currentVertexShader;
         std::cout << "Changed shader\n";
         shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
      }
      // if (is3Dmode) {
      //    shaderManager.getShader().setUniforms(std::make_tuple("oceanUpperBound", oceanUpperBound),
      //                                          std::make_tuple("sandLowerBound", sandLowerBound),
      //                                          std::make_tuple("sandUpperBound", sandUpperBound),
      //                                          std::make_tuple("grassLowerBound", grassLowerBound),
      //                                          std::make_tuple("grassUpperBound", grassUpperBound));
      // }
      shaderManager.getShader().setUniforms();

      // glUniform1f(glGetUniformLocation(shaderProgram.ID, "oceanUpperBound"), oceanUpperBound);
      // glUniform1f(glGetUniformLocation(shaderProgram.ID, "sandLowerBound"), sandLowerBound);
      // glUniform1f(glGetUniformLocation(shaderProgram.ID, "sandUpperBound"), sandUpperBound);
      // glUniform1f(glGetUniformLocation(shaderProgram.ID, "grassLowerBound"), grassLowerBound);
      // glUniform1f(glGetUniformLocation(shaderProgram.ID, "grassUpperBound"), grassUpperBound);

      // // Render ImGUI
      ImGui::Render();

      if (previousSeed != seed) {
         previousSeed = seed;
         myMesh = generateMeshFromSeed(seed);
      }

      Camera* camera = is3Dmode ? static_cast<Camera*>(&camera_3d) : static_cast<Camera*>(&camera_2d);

      camera->Inputs(window);
      camera->updateMatrix(45.0f, 0.1f, 100.0f);

      myMesh.Draw(shaderManager.getShader(), *camera);

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
