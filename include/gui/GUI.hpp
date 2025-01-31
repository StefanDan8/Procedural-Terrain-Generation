#ifndef GUI_CLASS_HPP
#define GUI_CLASS_HPP

#include "Window.hpp"
#include "ShaderManager.hpp"
#include "Mesh.hpp"
#include "Terrain.hpp"
#include "Fuse.hpp"
#include <vector>

class GUI {
   public:
   GUI(Window& window);

   void NewFrame();
   void Shutdown();
   void RenderDrawData();
   void DisplayGUI(Terrain& terrain, float fps);
   void DrawTerrain(Terrain& terrain, Camera& camera);
   void DeleteShaderManager();
   bool is3DModeActive() {
      return is3DMode;
   }
   bool isGUIHovered();
   void setGUIHovered(bool state);

   private:
   //ImGui Elements
   void RenderCommonImGui();
   void ShaderDropdown();
   void _3DInputControls();
   void _2DInputControls();
   void SaveToFile3D(Mesh& mesh);
   void SaveToFile2D(Mesh& mesh);
   void UserShaderParameters();
   void MeshSettings();
   bool InputUnsigned(const char* label, unsigned int* v, unsigned int step = 1, unsigned int step_fast = 10, ImGuiInputTextFlags flags = 0);
   void NoiseLayersGui(Terrain& terrain, Fuse& fuse);

   void Render3DImGui(Terrain& terrain, float fps);
   void Render2DImGui(Terrain& terrain, float fps);

   void SwitchShader(ShaderManager& shaderManager);

   bool guiHovered = false;
   bool is3DMode = false;
   bool switchedShaderRecently = false;
   const std::vector<std::vector<std::string>> shaders = {
      {"Beach2DShader.vert", "Mono2DShader.vert"}, //2D shaders
      {"beachShader.vert", "default.vert", "Fjord.vert"} // 3D shaders
   };
   std::string previousVertexShader = shaders[is3DMode][0];
   std::string currentVertexShader = previousVertexShader;
   std::string previousFragmentShader = "default.frag";
   std::string currentFragmentShader = previousFragmentShader;
   int previousSeed = 42;
   int seed = previousSeed;
   double flattenFactor = 2.0;
   double lastFlattenFactor = flattenFactor;
   int fpsPrintTimer = 0;
   float printFps = 0.0f;
   float fpsAvg = 0.0f;
   unsigned uselessIDcounter = 0;
   Window& window;
   ShaderManager shaderManager;
   Fuse fuse;
};
#endif