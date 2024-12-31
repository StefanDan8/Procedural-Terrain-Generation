
#define GLFW_INCLUDE_NONE

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "AppConfig.hpp"
#include "PerlinOOP.hpp"
#include "Renderer.hpp"
#include "UserInterface.hpp"

#include "Mesh.hpp"

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

float oceanUpperBound = 0.01f;
float sandLowerBound = 0.01f;
float sandUpperBound = 0.04f;
float grassLowerBound = 0.04f;
float grassUpperBound = 0.1f;

int previousSeed = 42;
int seed = previousSeed;

void RenderImGui() {
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

   ImGui::SliderFloat("Ocean Upper Bound", &oceanUpperBound, -1.0f, 0.5f);
   ImGui::SliderFloat("Sand Lower Bound", &sandLowerBound, -0.1f, 0.5f);
   ImGui::SliderFloat("Sand Upper Bound", &sandUpperBound, -0.1f, 0.5f);
   ImGui::SliderFloat("Grass Lower Bound", &grassLowerBound, -0.1f, 0.5f);
   ImGui::SliderFloat("Grass Upper Bound", &grassUpperBound, -0.1f, 0.5f);
   ImGui::Text("Mesh Settings");
   ImGui::InputInt("Seed", &seed);
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
   noise.fill(result);
   perlin::matrix filter(sizeX, std::vector<double>(sizeY, 0.0));
   std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 5), std::make_pair(120, 5), std::make_pair(60, 10), std::make_pair(30, 1)};
   perlin::PerlinNoise2D noiseFilter(sizeX, sizeY, filterParams);
   noiseFilter.fill(filter);

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

   Shader shaderProgram("beachShader.vert", "default.frag");

   // Mesh myMesh(normalized);
   Mesh myMesh = generateMeshFromSeed(42);
   //ComputeNormals(myMesh);
   //PrintMesh(myMesh);

   glEnable(GL_DEPTH_TEST);

   Camera camera((int) RENDER_WIDTH, framebufferHeight, glm::vec3(0.0f, 0.5f, 2.0f));

   while (!glfwWindowShouldClose(window)) {
      // Start ImGUI frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ImGui::SetNextWindowSize(ImVec2(IMGUI_WIDTH, framebufferHeight), ImGuiCond_Always);
      ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

      RenderImGui();
      glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      shaderProgram.Activate();

      glUniform1f(glGetUniformLocation(shaderProgram.ID, "oceanUpperBound"), oceanUpperBound);
      glUniform1f(glGetUniformLocation(shaderProgram.ID, "sandLowerBound"), sandLowerBound);
      glUniform1f(glGetUniformLocation(shaderProgram.ID, "sandUpperBound"), sandUpperBound);
      glUniform1f(glGetUniformLocation(shaderProgram.ID, "grassLowerBound"), grassLowerBound);
      glUniform1f(glGetUniformLocation(shaderProgram.ID, "grassUpperBound"), grassUpperBound);

      // // Render ImGUI
      ImGui::Render();

      if (previousSeed != seed) {
         previousSeed = seed;
         myMesh = generateMeshFromSeed(seed);
      }

      camera.Inputs(window);
      camera.width = (int) (0.75f * framebufferWidth);
      camera.height = (int) framebufferHeight;
      camera.updateMatrix(45.0f, 0.1f, 100.0f);

      myMesh.Draw(shaderProgram, camera);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   shaderProgram.Delete();

   glfwDestroyWindow(window);
   // Shutdown ImGUI
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
   glfwTerminate();
   return 0;
}
