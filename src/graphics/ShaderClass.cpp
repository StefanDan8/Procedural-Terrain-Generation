#include "ShaderClass.hpp"

std::string get_file_contents(std::string filename) {
   std::ifstream in(filename, std::ios::binary);
   if (in) {
      std::string contents;
      in.seekg(0, std::ios::end);
      contents.resize(in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], contents.size());
      in.close();
      return contents;
   }
   throw(errno);
}

Shader::Shader(std::string vertexFile, std::string fragmentFile) {
   std::string vertexCode = get_file_contents("../shaders/" + vertexFile);
   std::string fragmentCode = get_file_contents("../shaders/" + fragmentFile);

   const char* vertexSource = vertexCode.c_str();
   const char* framgentSource = fragmentCode.c_str();

   GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader, 1, &vertexSource, NULL);
   glCompileShader(vertexShader);

   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &framgentSource, NULL);
   glCompileShader(fragmentShader);

   ID = glCreateProgram();
   glAttachShader(ID, vertexShader);
   glAttachShader(ID, fragmentShader);

   glLinkProgram(ID);
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
}

void Shader::processUniform(const GLchar* name, float value) {
   Activate();
   glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::Activate() {
   glUseProgram(ID);
}

void Shader::Delete() {
   glDeleteProgram(ID);
}