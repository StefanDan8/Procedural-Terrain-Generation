#include "ShaderManager.hpp"

const std::string defaultFrag = "default.frag";

ShaderManager::ShaderManager(const std::vector<std::vector<std::string>> &shaders)
   : shader(nullptr) {
   for (auto &shader: shaders) {
      for (const auto& shaderName: shader) {
         std::cout << "Initialising shader: " << shaderName << "/" << defaultFrag << std::endl;
         initialisedShaders[shaderName] = Shader(shaderName, defaultFrag);
         userShaderParams[shaderName] = std::unordered_map<std::string, float>();
         for (unsigned i = 0; i < initialisedShaders[shaderName].userFloatUniforms.size(); ++i) {
            userShaderParams[shaderName][initialisedShaders[shaderName].userFloatUniforms[i]] = initialisedShaders[shaderName].userFloatValues[i];
         }
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

Shader& ShaderManager::getShader() {
   return *shader;
}

std::unordered_map<std::string, float>* ShaderManager::getUserShaderParams() {
   return &userShaderParams[shader->name];
}