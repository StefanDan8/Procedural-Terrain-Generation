#ifndef SHADER_MANAGER_CLASS_HPP
#define SHADER_MANAGER_CLASS_HPP

#include "ShaderClass.hpp"
class ShaderManager {
   public:
   ShaderManager(Shader& some_shader);
   ShaderManager(std::string vertexFile, std::string fragmentFile);
   void SwitchShader(std::string vertexFile, std::string fragmentFile);
   void Delete();
   Shader& getShader();

   private:
   Shader shader;
};
#endif