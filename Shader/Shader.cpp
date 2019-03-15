#include "Shader.h"


int Shader::getUniformLocation(const char* name)
{

}

bool Shader::fileExists(const std::string &filename)
{

}

bool compileShaderFromFile(const char *filename, GLSLShader::GLSLShaderType type)
{

}

bool Shader::link()
{

}

void Shader::use()
{

}
  
int  Shader::getHandle()
{

}
bool Shader::isLinked()
{

}
  
void Shader::bindAttribLocation(uint16_t location, const char* name)
{

}
void Shader::bindFragDataLocation(uint16_t location, const char *name)
{

}
    
void Shader::setUniform(const char* name, float x, float y, float z)
{

}
   
void Shader::setUniform(const char* name, const glm::vec3 &v)
{

}
void Shader::setUniform(const char* name, const glm::vec4 &v)
{

}
    
void Shader::setUniform(const char* name, const glm::mat4 &m)
{

}
void Shader::setUniform(const char* name, const glm::mat3 &m)
{

}
   
void Shader::setUniform(const char* name, float val)
{

}
void Shader::setUniform(const char* name, int val)
{

}
   
void Shader::setUniform(const char* name, bool val)
{

}