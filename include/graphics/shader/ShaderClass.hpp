#ifndef MY_NICE_SHADERS_HPP
#define MY_NICE_SHADERS_HPP

#include <cerrno>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>

std::vector<std::string> parse_uniform_float_names(const std::string &contents);

void parse_uniform_float_names_and_values(const std::string &contents,
                                          std::vector<std::string> &uniformNames,
                                          std::vector<float> &uniformValues);

std::string get_file_contents(std::string filename);

class Shader {
public:
    GLuint ID;
    std::string name;
    std::vector<std::string> userFloatUniforms;
    std::vector<float> userFloatValues;

    /**
     * Do not use this constructor, for some reason it just won't build without.
     */
    Shader();

    Shader(const std::string &vertexFile, const std::string &fragmentFile);

    void processUniform(const GLchar *name, float value);

    void setUniforms();

    void Activate();
    void Delete();
};

#endif
