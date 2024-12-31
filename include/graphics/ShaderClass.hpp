#ifndef MY_NICE_SHADERS_HPP
#define MY_NICE_SHADERS_HPP

#include <cerrno>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <glad/glad.h>

std::string get_file_contents(std::string filename);

class Shader {
   public:
   GLuint ID;
   Shader(std::string vertexFile, std::string fragmentFile);

   void Activate();
   void Delete();
};

#endif