#include "ShaderManager.hpp"

const std::string defaultFrag = "default.frag";

ShaderManager::ShaderManager(const std::vector<std::vector<std::string>> &shaders)
   : shader(nullptr) {
   for (auto &shader: shaders) {
      for (const auto& shaderName: shader) {
         initialisedShaders[shaderName] = Shader(shaderName, defaultFrag);
      }
   }
   shader = &initialisedShaders[shaders[0][0]]; // Default to the first shader
}


void ShaderManager::SwitchShader(const std::string& vertexFile) {
   glUseProgram(0); // Unbind the shader
   shader = &initialisedShaders[vertexFile];
   shader->Activate();
}

void ShaderManager::Delete() {
   shader->Delete();
}

Shader& ShaderManager::getCurrentShader() {
   return *shader;
}

Shader* ShaderManager::getShader(const std::string& vertexFile) {
   return &initialisedShaders[vertexFile];
}