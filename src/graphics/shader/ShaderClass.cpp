#include "ShaderClass.hpp"

std::vector<std::string> parse_uniform_float_names(const std::string& contents) {
   std::vector<std::string> uniformNames;
   std::size_t startPos = contents.find("//--Uniforms for user");
   if (startPos != std::string::npos) {
      // 'uniform float <name>;'
      std::regex uniformFloatRegex(R"(uniform\s+float\s+(\w+);)");
      std::smatch match;

      std::string::const_iterator searchStart(contents.begin() + startPos);
      while (std::regex_search(searchStart, contents.cend(), match, uniformFloatRegex)) {
         uniformNames.push_back(match[1]); // get group 1
         searchStart = match.suffix().first;
      }
   }
   for (const auto& name : uniformNames) {
      std::cout << "Found uniform float: " << name << '\n';
   }
   return uniformNames;
}

void parse_uniform_float_names_and_values(const std::string& contents,
                                          std::vector<std::string>& uniformNames,
                                          std::vector<float>& uniformValues) {
   uniformNames.resize(0);
   uniformValues.resize(0);
   std::size_t startPos = contents.find("//--Uniforms for user");
   if (startPos != std::string::npos) {
      std::regex uniformFloatRegex(R"(uniform\s+float\s+(\w+);\s*(?://\s*([\d.]+))?)");
      std::smatch match;

      std::string::const_iterator searchStart(contents.begin() + startPos);
      while (std::regex_search(searchStart, contents.cend(), match, uniformFloatRegex)) {
         uniformNames.push_back(match[1]); // Capture group 1 contains the variable name
         if (match[2].matched) {
            // Convert the captured value to float
            uniformValues.push_back(std::stof(match[2].str()));
         } else {
            uniformValues.push_back(0.0f); // Default value if no comment is found
         }
         searchStart = match.suffix().first;
      }
   }
   for (unsigned i = 0; i < uniformNames.size(); ++i) {
      std::cout << "Found uniform float: " << uniformNames[i] << " with default value (0.0 if not found) " << uniformValues[i] << "\n";
   }
}

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
   //throw(errno);
   throw std::system_error(errno, std::generic_category(), "Error reading shader files " + filename);
}

Shader::Shader() {
   ID = -1;
   name = "";
}

Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile) {
   std::string vertexCode = get_file_contents(std::string(SHADER_ROOT) + "/" + vertexFile);
   std::string fragmentCode = get_file_contents(std::string(SHADER_ROOT) + "/" + fragmentFile);

   // userFloatUniforms = parse_uniform_float_names(vertexCode);
   // userFloatValues.resize(userFloatUniforms.size());
   parse_uniform_float_names_and_values(vertexCode, userFloatUniforms, userFloatValues);

   const char* vertexSource = vertexCode.c_str();
   const char* framgentSource = fragmentCode.c_str();

   GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader, 1, &vertexSource, NULL);
   glCompileShader(vertexShader);

   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &framgentSource, NULL);
   glCompileShader(fragmentShader);

   ID = glCreateProgram();
   name = vertexFile;
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

void Shader::setUniforms() {
   Activate();
   for (unsigned i = 0; i < userFloatUniforms.size(); ++i) {
      glUniform1f(glGetUniformLocation(ID, userFloatUniforms[i].c_str()), userFloatValues[i]);
   }
}

void Shader::Activate() {
   glUseProgram(ID);
}

void Shader::Delete() {
   glDeleteProgram(ID);
}