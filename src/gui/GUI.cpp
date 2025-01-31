#include "GUI.hpp"

GUI::GUI(Window& window) : window(window), shaderManager(previousVertexShader, previousFragmentShader), fuse(200, 8, 4) {
   if (!window.getWindow()) {
      throw std::runtime_error("GLFW window is null in GUI constructor!");
   }
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls (optional)
   ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
   ImGui_ImplOpenGL3_Init("#version 330");
   ImGui::StyleColorsDark();
}

void GUI::NewFrame() {
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   ImGui::SetNextWindowSize(ImVec2(*window.getImguiWidth(), window.getFrameBufferHeight()), ImGuiCond_Always);
   ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
}

void GUI::RenderDrawData() {
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
   window.SwapBuffers();
}

void GUI::Shutdown() {
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
}

//ImGui Elements
void GUI::RenderCommonImGui() {
   ImGui::Text("Pick display mode:");

   if (ImGui::Button("2D Mode")) {
      is3DMode = false;
      switchedShaderRecently = true;
   }
   ImGui::SameLine();
   if (ImGui::Button("3D Mode")) {
      is3DMode = true;
      switchedShaderRecently = true;
   }
}

void GUI::ShaderDropdown() {
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

void GUI::SwitchShader(ShaderManager& shaderManager) {
   if (switchedShaderRecently) {
      switchedShaderRecently = false;
      currentVertexShader = previousVertexShader = shaders[is3DMode][0];
      shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
   }

   if (currentVertexShader.compare(previousVertexShader)) {
      previousVertexShader = currentVertexShader;
      std::cout << "Changed shader\n";
      shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
   }

   shaderManager.getShader().setUniforms();
}

void GUI::_3DInputControls() {
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

void GUI::_2DInputControls() {
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
void GUI::SaveToFile3D(Mesh& mesh) {
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
void GUI::SaveToFile2D(Mesh& mesh) {
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
void GUI::UserShaderParameters(ShaderManager& manager) {
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
void GUI::MeshSettings() {
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

bool GUI::InputUnsigned(const char* label, unsigned int* v, unsigned int step, unsigned int step_fast, ImGuiInputTextFlags flags) {
   int value = static_cast<int>(*v); // Cast to signed for ImGui input
   bool changed = ImGui::InputInt(label, &value, static_cast<int>(step), static_cast<int>(step_fast), flags);

   // Enforce unsigned constraint
   if (value < 0) {
      value = 0; // Clamp to zero if negative
   }
   *v = static_cast<unsigned int>(value); // Update the original unsigned value
   return changed;
}

void GUI::NoiseLayersGui(Terrain& terrain, Fuse& fuse) {
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
void GUI::Render3DImGui(Terrain& terrain, float fps) {
   fpsPrintTimer++;
   fpsAvg += fps;
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   RenderCommonImGui();
   ImGui::Text("\n3D Mode\n\n");

   UserShaderParameters(shaderManager);
   MeshSettings();
   ShaderDropdown();
   NoiseLayersGui(terrain, fuse);
   SaveToFile3D(terrain.getMesh());
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
void GUI::Render2DImGui(Terrain& terrain, float fps) {
   fpsPrintTimer++;
   fpsAvg += fps;
   ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   RenderCommonImGui();
   ImGui::Text("\n2D Mode\n\n");

   UserShaderParameters(shaderManager);
   MeshSettings();

   ShaderDropdown();

   SaveToFile2D(terrain.getMesh());
   _2DInputControls();
   if (fpsPrintTimer > 99) {
      fpsPrintTimer = 0;
      printFps = fpsAvg / 100;
      fpsAvg = 0.0;
   }
   ImGui::Text("FPS: %1.f", printFps);
   ImGui::End();
}

void GUI::DisplayGUI(Terrain& terrain, float elapsedSinceLastFrame) {
   if (is3DMode) {
      Render3DImGui(terrain, 1.0f / elapsedSinceLastFrame);
   } else {
      Render2DImGui(terrain, 1.0f / elapsedSinceLastFrame);
   }

   if (switchedShaderRecently) {
      switchedShaderRecently = false;
      currentVertexShader = previousVertexShader = shaders[is3DMode][0];
      shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
   }

   if (currentVertexShader.compare(previousVertexShader)) {
      previousVertexShader = currentVertexShader;
      std::cout << "Changed shader\n";
      shaderManager.SwitchShader(currentVertexShader, currentFragmentShader);
   }

   shaderManager.getShader().setUniforms();
   glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
}

void GUI::DrawTerrain(Terrain& terrain, Camera& camera) {
   terrain.Draw(shaderManager.getShader(), camera);
}

void GUI::DeleteShaderManager() {
   shaderManager.Delete();
}
