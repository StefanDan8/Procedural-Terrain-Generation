#include "GUI.hpp"

GUI::GUI(Window& window) : window(window) {
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
