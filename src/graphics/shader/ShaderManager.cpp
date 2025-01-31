#include "ShaderManager.hpp"

ShaderManager::ShaderManager(std::string vertexFile, std::string fragmentFile) : shader(Shader(vertexFile, fragmentFile)) {
   shader.Activate();
}

ShaderManager::ShaderManager(Shader& some_shader) : shader(some_shader) {
   shader.Activate();
}

void ShaderManager::SwitchShader(std::string vertexFile, std::string fragmentFile) {
   shader.Delete(); // Delete previous Shader
   shader = Shader(vertexFile, fragmentFile);
   shader.Activate();
}

void ShaderManager::Delete() {
   shader.Delete();
}

Shader& ShaderManager::getShader() {
   return shader;
}