#ifndef SHADER_MANAGER_CLASS_HPP
#define SHADER_MANAGER_CLASS_HPP

#include <unordered_map>
#include "ShaderClass.hpp"

class ShaderManager {
   Shader* shader;
   std::unordered_map<std::string, Shader> initialisedShaders;

public:
   ShaderManager(const std::vector<std::vector<std::string>> &shaders);
   void SwitchShader(const std::string& vertexFile);
   void Delete();
   Shader& getCurrentShader();
   Shader* getShader(const std::string& vertexFile);
};
#endif