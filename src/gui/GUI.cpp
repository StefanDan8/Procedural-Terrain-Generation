#include "GUI.hpp"

GUI::GUI(Window& window) : window(window), shaderManager(shaders), fuse(200, 8, 4) {
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
void GUI::RenderCommonImGui(Terrain& terrain, float fps) {
   uselessIDcounter = 0; // DON'T REMOVE EXTREMELY IMPORTANT ;)
   fpsPrintTimer++;
   fpsAvg += fps;
   ImGui::Begin(GUI_IDENTIFIER, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
   setGUIHovered(ImGui::IsWindowHovered());

   DisplayMode();
   MeshSettings();
   if (is3DMode) {
      ShaderDropdown3D();
   } else {
      ShaderDropdown2D();
   }

   ImGui::Text("\n");
   UserShaderParameters();
   NoiseLayersGui(terrain, fuse);
   JSON_IO(terrain);
}

void GUI::DisplayMode() {
   const std::string title = "DISPLAY MODE = " + std::string(is3DMode ? "3D" : "2D");
   ImGui::Text(title.c_str());

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
void GUI::ShaderDropdown2D() {
   ImGui::Text("\nSelect a Shader");
   ImGui::SetNextItemWidth(160.0f);
   // is3DMode is a boolean, which really just means 0 for 2D, 1 for 3D
   if (ImGui::BeginCombo("##xa", shaders[0][currentItem2D].c_str())) {
      for (unsigned i = 0; i < shaders[0].size(); i++) {
         bool isSelected = (currentItem2D == i);
         if (ImGui::Selectable(shaders[0][i].c_str(), isSelected)) {
            currentItem2D = i; // Update selected item index
            currentVertexShader = shaders[0][i];
         }
         // Set the initial focus when opening the combo (scrolling to current item)
         if (isSelected) {
            ImGui::SetItemDefaultFocus();
         }
      }
      ImGui::EndCombo();
   }
}

void GUI::ShaderDropdown3D() {
   ImGui::Text("\nSelect a Shader");
   ImGui::SetNextItemWidth(160.0f);
   // is3DMode is a boolean, which really just means 0 for 2D, 1 for 3D
   if (ImGui::BeginCombo("##xa", shaders[1][currentItem3D].c_str())) {
      for (unsigned i = 0; i < shaders[1].size(); i++) {
         bool isSelected = (currentItem3D == i);
         if (ImGui::Selectable(shaders[1][i].c_str(), isSelected)) {
            currentItem3D = i; // Update selected item index
            currentVertexShader = shaders[1][i];
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
      currentVertexShader = previousVertexShader = shaders[is3DMode][is3DMode ? currentItem3D : currentItem2D];
      shaderManager.SwitchShader(currentVertexShader);
   }

   if (currentVertexShader.compare(previousVertexShader)) {
      previousVertexShader = currentVertexShader;
      std::cout << "Changed shader\n";
      shaderManager.SwitchShader(currentVertexShader);
   }

   shaderManager.getCurrentShader().setUniforms();
}

void GUI::YesNoPopup(const std::string title, const std::string message, const std::function<void()>& yesCallback) {
   if (ImGui::BeginPopupModal(title.c_str())) {
      ImGui::Text("ARE YOU SURE?\n\n%s", message.c_str());
      if (ImGui::Button("Yes")) {
         yesCallback();
         ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("No")) {
         ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
   }
}

void GUI::OkPopup(const std::string title, const std::string message, const std::function<void()> &okCallback) {
   if (ImGui::BeginPopupModal(title.c_str())) {
      ImGui::Text(message.c_str());
      if (ImGui::Button("Ok")) {
         okCallback();
         ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
   }
}


void GUI::_3DInputControls() {
   static bool isClicked3 = false;
   if (ImGui::Button("Input Controls")) {
      isClicked3 = !isClicked3;
   }

   if (isClicked3 || ImGui::IsItemHovered()) {
      ImGui::SetTooltip(R"(
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
}

void GUI::_2DInputControls() {
   static bool isClicked2 = false;
   if (ImGui::Button("Input Controls")) {
      isClicked2 = !isClicked2;
   }

   if (isClicked2 || ImGui::IsItemHovered()) {
      ImGui::SetTooltip(R"(
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
}

/**
 * Renders the save to file options for 3D mode in the ImGui window. (obj)
 * @param mesh Mesh to be saved to a file
 * @author SD
 */
void GUI::SaveToFile3D(Mesh& mesh) {
   ImGui::Text("\nSave Current Object to .obj\n");
   static char _user_save_path[256] = "";
   ImGui::InputText("Filename", _user_save_path, sizeof(_user_save_path));
   auto filename = std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path + ".obj";
   if (ImGui::Button("Save to .obj")) {
      if (std::filesystem::exists(filename)) {
         // Create pop up to ask if they want to overwrite the file
         ImGui::OpenPopup("ConfirmOBJOverwrite");
      } else {
         ExportToObj(mesh, filename);
         operationCompleted = true;
      }
   }

   YesNoPopup("ConfirmOBJOverwrite", "You already have a file named this, do you want to overwrite it?", [&]() {
      ExportToObj(mesh, filename);
      operationCompleted = true;
   });
}

/**
 * Renders the save to file options for 2D mode in the ImGui window. (png and ppm)
 * @param mesh Mesh to be saved to a file
 * @author PK
 */
void GUI::SaveToFile2D(Mesh& mesh) {
   ImGui::Text("\nSave Current Image");
   static char _user_save_path[256] = "";
   ImGui::InputText("Filename", _user_save_path, sizeof(_user_save_path));
   std::string filename = std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path;
   if (ImGui::Button("Save to .png")) {
      if (std::filesystem::exists(filename + ".png")) {
         // Create pop up to ask if they want to overwrite the file
         ImGui::OpenPopup("ConfirmPNGOverwrite");
      } else {
         mesh.exportToPNG(filename + ".png");
         operationCompleted = true;
      }
   }
   ImGui::SameLine();
   if (ImGui::Button("Save to .ppm")) {
      if (std::filesystem::exists(filename + ".ppm")) {
         // Create pop up to ask if they want to overwrite the file
         ImGui::OpenPopup("ConfirmPPMOverwrite");
      } else {
         mesh.exportToPPM(filename + ".ppm");
         operationCompleted = true;
      }
   }

   YesNoPopup("ConfirmPNGOverwrite", "You already have a file named this, do you want to overwrite it?", [&]() {
      mesh.exportToPNG(filename + ".png");
      operationCompleted = true;
   });

   YesNoPopup("ConfirmPPMOverwrite", "You already have a file named this, do you want to overwrite it?", [&]() {
      mesh.exportToPPM(filename + ".ppm");
      operationCompleted = true;
   });
}

/**
 * Saves the current settings to a JSON file.
 * @param terrain Required to pull noise and baseline params
 * @param filename Absolute path to the JSON file
 */
void GUI::SaveJSON(Terrain& terrain, std::string filename) {
   nlohmann::json j;
   j["seed"] = seed;
   j["flattenFactor"] = flattenFactor;
   j["is3DMode"] = is3DMode;
   j["shader"] = currentVertexShader;
   j["shaderParams"] = nlohmann::json::object();

   for (unsigned i = 0; i < shaderManager.getCurrentShader().userFloatValues.size(); i++) {
      j["shaderParams"][shaderManager.getCurrentShader().userFloatUniforms[i]] = shaderManager.getCurrentShader().userFloatValues[i];
   }

   j["noiseParams"] = nlohmann::json::array();
   for (auto& layerParam : terrain.getNoiseParams()) {
      j["noiseParams"].push_back({{"chunkSize", layerParam.first}, {"weight", layerParam.second}});
   }
   j["baselineParams"] = nlohmann::json::array();
   for (auto& layerParam : terrain.getBaselineParams()) {
      j["baselineParams"].push_back({{"chunkSize", layerParam.first}, {"weight", layerParam.second}});
   }

   std::ofstream file(filename, std::ios::trunc);
   file << j.dump();
}

/**
 * Loads the settings from a JSON file.
 * @param terrain Required to update noise and baseline params
 * @param filename Absolute path to the JSON file
 */
void GUI::LoadJSON(Terrain& terrain, std::string filename) {
   std::ifstream file(filename);
   if (!file.is_open()) {
      std::cerr << "Failed to open file: " << filename << std::endl;
      return;
   }
   nlohmann::json j;
   file >> j;

   previousSeed = seed;
   seed = j["seed"];
   previousVertexShader = currentVertexShader;
   currentVertexShader = j["shader"];
   shaderManager.SwitchShader(j["shader"]);
   flattenFactor = j["flattenFactor"];
   is3DMode = j["is3DMode"];

   // Noise and baseline parameters
   auto noiseParams = j["noiseParams"];
   for (unsigned i = 0; i < noiseParams.size(); ++i) {
      terrain.getNoiseParams()[i].first = noiseParams[i]["chunkSize"];
      terrain.getNoiseParams()[i].second = noiseParams[i]["weight"];
   }
   auto baselineParams = j["baselineParams"];
   for (unsigned i = 0; i < baselineParams.size(); ++i) {
      terrain.getBaselineParams()[i].first = baselineParams[i]["chunkSize"];
      terrain.getBaselineParams()[i].second = baselineParams[i]["weight"];
   }

   Shader* shader = shaderManager.getShader(j["shader"]);
   for (unsigned i = 0; i < shader->userFloatValues.size(); i++) {
      shader->userFloatValues[i] = j["shaderParams"][shader->userFloatUniforms[i]];
   }
}

/**
 * UI Logic for saving/loading to/from JSON files.
 * @author PK
 */
void GUI::JSON_IO(Terrain& terrain) {
   ImGui::Text("\nSave Current Settings");
   static char _user_save_path[256] = "";
   ImGui::InputText("JSON Filename", _user_save_path, sizeof(_user_save_path));
   auto filename = std::string(OUTPUT_FOLDER_PATH) + "/" + _user_save_path + ".json";
   if (ImGui::Button("Save to .json")) {
      // Check file exists
      if (std::filesystem::exists(filename)) {
         // Create pop up to ask if they want to overwrite the file
         ImGui::OpenPopup("ConfirmJSONOverwrite");
      } else {
         SaveJSON(terrain, filename);
         operationCompleted = true;
      }
   }
   ImGui::SameLine();
   if (ImGui::Button("Load")) {
      // Create pop up to ask if they want to load the file
      ImGui::OpenPopup("ConfirmJSONLoad");
   }

   YesNoPopup("ConfirmJSONLoad", "This will irreversibly overwrite your current settings!", [&]() {
      LoadJSON(terrain, filename);
      operationCompleted = true;
   });
   YesNoPopup("ConfirmJSONOverwrite", "You already have a file named this, do you want to overwrite it?", [&]() {
      SaveJSON(terrain, filename);
      operationCompleted = true;
   });
}

/**
 * Renders the user shader parameters in the ImGui window.
 * @author SD, PK (extracted method)
 */
void GUI::UserShaderParameters() {
   if (ImGui::CollapsingHeader("User Shader Parameters")) {
      const Shader shader = shaderManager.getCurrentShader();
      for (unsigned i = 0; i < shader.userFloatValues.size(); i++) {
         ImGui::PushID(uselessIDcounter++);
         ImGui::SetNextItemWidth(90.f);
         ImGui::SliderFloat("", &(shaderManager.getCurrentShader().userFloatValues[i]), -1.0f, 1.0f);
         ImGui::PopID();
         ImGui::SameLine();
         ImGui::PushID(uselessIDcounter++);
         if (ImGui::Button("-")) {
            shaderManager.getCurrentShader().userFloatValues[i] -= 0.002f;
         }
         ImGui::SameLine();
         ImGui::PopID();
         ImGui::PushID(uselessIDcounter++);
         if (ImGui::Button("+")) {
            shaderManager.getCurrentShader().userFloatValues[i] += 0.002f;
         }
         ImGui::PopID();
         ImGui::SameLine();
         ImGui::Text(shaderManager.getCurrentShader().userFloatUniforms[i].c_str());
      }
   }
}

/**
 * Renders the common mesh settings in the ImGui window.
 * @author SD, PK (extracted method)
 */
void GUI::MeshSettings() {
   ImGui::Text("\nMesh Settings");
   ImGui::SetNextItemWidth(90.f);
   ImGui::InputInt("Seed", &seed);

   ImGui::PushItemWidth(90.f);
   ImGui::InputDouble("##xx", &flattenFactor, 0.2, 1.0, "%.2f");

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
   if (ImGui::CollapsingHeader("Noise Parameters")) {
      unsigned index = 0;
      ImGui::Text("Chunk Size       Weight");
      for (auto& layerParam : terrain.getNoiseParams()) {
         ImGui::PushID(uselessIDcounter++);
         ImGui::SetNextItemWidth(110.f);
         if (InputUnsigned("##xx", &layerParam.first, 10, 100)) {
            fuse.planLayerUpdate(index, NOISE_LAYER, CHUNK_SIZE);
         }
         ImGui::PopID();
         ImGui::SameLine();
         ImGui::PushID(uselessIDcounter++);
         ImGui::SetNextItemWidth(110.f);
         if (ImGui::InputDouble("##xx", &layerParam.second, 1.0, 10.0, "%.1f")) {
            fuse.planLayerUpdate(index, NOISE_LAYER, WEIGHT);
         }
         ImGui::PopID();
         index++;
      }
      index = 0;
      ImGui::Text("Baseline Noise Parameters");
      for (auto& layerParam : terrain.getBaselineParams()) {
         ImGui::PushID(uselessIDcounter++);
         ImGui::SetNextItemWidth(110.f);
         if (InputUnsigned("##xx", &layerParam.first, 10, 100)) {
            fuse.planLayerUpdate(index, BASELINE_LAYER, CHUNK_SIZE);
         }
         ImGui::PopID();
         ImGui::SameLine();
         ImGui::PushID(uselessIDcounter++);
         ImGui::SetNextItemWidth(110.f);
         if (ImGui::InputDouble("##xx", &layerParam.second, 1.0, 10.0, "%.1f")) {
            fuse.planLayerUpdate(index, BASELINE_LAYER, WEIGHT);
         }
         ImGui::PopID();
         index++;
      }
   }
}

void GUI::FPSDisplay() {
   if (fpsPrintTimer > 99) {
      fpsPrintTimer = 0;
      printFps = fpsAvg / 100;
      fpsAvg = 0.0;
   }
   ImGui::Text("FPS: %1.f", printFps);
}

/**
 * Renders the full ImGui window for the 2D mode.
 * @param terrain Required for getting terrain data to modify/save
 * @param fps Calculated fps to display
 * @author SD, PK (extracted method, modifications)
 */
void GUI::Render3DImGui(Terrain& terrain, float fps) {
   RenderCommonImGui(terrain, fps);
   SaveToFile3D(terrain.getMesh());

   ImGui::Text("\n");
   _3DInputControls();
   ImGui::SameLine();
   FPSDisplay();

   ImGui::End();
}

/**
 * Renders the full ImGui window for the 2D mode.
 * @param terrain Required for getting terrain data to modify/save
 * @param fps Calculated fps to display
 * @author PK, SD
 */
void GUI::Render2DImGui(Terrain& terrain, float fps) {
   RenderCommonImGui(terrain, fps);
   SaveToFile2D(terrain.getMesh());

   ImGui::Text("\n");
   _2DInputControls();
   ImGui::SameLine();
   FPSDisplay();

   ImGui::End();
}

void GUI::DisplayGUI(Terrain& terrain, float elapsedSinceLastFrame) {
   if (is3DMode) {
      Render3DImGui(terrain, 1.0f / elapsedSinceLastFrame);
   } else {
      Render2DImGui(terrain, 1.0f / elapsedSinceLastFrame);
   }

   if (operationCompleted) {
      ImGui::OpenPopup("SuccessfulOperation");
      operationCompleted = false;
   }
   OkPopup("SuccessfulOperation", "The operation was completed successfully!", []() {});

   if (switchedShaderRecently) {
      switchedShaderRecently = false;
      currentVertexShader = previousVertexShader = shaders[is3DMode][is3DMode ? currentItem3D : currentItem2D];
      shaderManager.SwitchShader(currentVertexShader);
   }

   if (currentVertexShader.compare(previousVertexShader)) {
      previousVertexShader = currentVertexShader;
      std::cout << "Changed shader\n";
      shaderManager.SwitchShader(currentVertexShader);
   }

   shaderManager.getCurrentShader().setUniforms();
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
   terrain.Draw(shaderManager.getCurrentShader(), camera);
}

void GUI::DeleteShaderManager() {
   shaderManager.Delete();
}

bool GUI::isGUIHovered() {
   return guiHovered;
}
void GUI::setGUIHovered(bool state) {
   guiHovered = state;
}
