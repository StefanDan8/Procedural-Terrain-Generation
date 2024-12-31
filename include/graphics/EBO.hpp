#ifndef EBO_CLASS_HPP
#define EBO_CLASS_HPP

#include <vector>
#include <glad/glad.h>

class EBO {
   public:
   GLuint ID;
   EBO(std::vector<GLuint>& indices);

   void Bind();
   void Unbind();
   void Delete();
};
#endif