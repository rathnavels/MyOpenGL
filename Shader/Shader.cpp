#include <fstream>
#include <sstream>

#include "Shader.h"


bool Shader::fileExists(const std::string &filename)
{
  std::ifstream f(filename);
    return f.good();
}

bool Shader::compileShaderFromFile(const char *filename, GLSLShader::GLSLShaderType type)
{
  std::string code;
  std::ifstream shaderFile;

  // Load and Compile
  if(fileExists(filename))
    shaderFile.open(filename);
  else
  {
    std::cerr << "File Not found\n";
    return false;
  }

  std::stringstream shaderStream;
  shaderStream << shaderFile.rdbuf();
  shaderFile.close();

  code = shaderStream.str();

    if (handle <= 0) 
        handle = glCreateProgram();
  
  GLuint shaderHandle = glCreateShader(type);
  const char *c_code = code.c_str();
  glShaderSource(shaderHandle, 1, &c_code, NULL);

  glCompileShader(shaderHandle);

  // Error Checking
  int result;
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
  if(result == GL_FALSE)
  {
    int length = 0;
    std::string logString;
    glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
    if(length > 0)
    {
      char *c_log = new char[length]   ;
      int written = 0;
      glGetShaderInfoLog(shaderHandle, length, &written, c_log);
      logString = c_log;
      delete[] c_log;
    }

    std::cout << std::endl << logString << std::endl;
  }
  else
    glAttachShader(handle, shaderHandle);

  return true;
}

bool Shader::link()
{
  glLinkProgram(handle);
  return true;
}

void Shader::use()
{
  glUseProgram(handle);
}
  
int  Shader::getHandle()
{
  return handle;
}

bool Shader::isLinked()
{
  return linked;
}

int Shader::getUniformLocation(const char* name)
{
  return 0;
}
    
void Shader::setUniform(const char* name, float x, float y, float z)
{
  glUniform3f(glGetUniformLocation(handle, name), x, y, z);
}
   
void Shader::setUniform(const char* name, const glm::vec3 &v)
{
  glUniform3f(glGetUniformLocation(handle, name), v.x, v.y, v.z);
}

void Shader::setUniform(const char* name, const glm::vec4 &v)
{
  glUniform4f(glGetUniformLocation(handle, name), v.x, v.y, v.z, v.w);
}
    
void Shader::setUniform(const char* name, const glm::mat4 &m)
{
  glUniformMatrix4fv(glGetUniformLocation(handle, name),1,GL_FALSE, glm::value_ptr(m));
}

void Shader::setUniform(const char* name, const glm::mat3 &m)
{
  glUniformMatrix3fv(glGetUniformLocation(handle, name),1,GL_FALSE, glm::value_ptr(m));
}
   
void Shader::setUniform(const char* name, float val)
{
  glUniform1f(glGetUniformLocation(handle, name), val);
}

void Shader::setUniform(const char* name, int val)
{
  glUniform1i(glGetUniformLocation(handle, name), val);
}
   
void Shader::setUniform(const char* name, bool val)
{
  glUniform1i(glGetUniformLocation(handle, name), val);
}

void Shader::bindAttribLocation(uint16_t location, const char* name)
{
  
}

void Shader::bindFragDataLocation(uint16_t location, const char *name)
{

}


Shader::Shader(const char *vS, const char *fS)
{
  compileShaderFromFile(vS,GLSLShader::VERTEX);
  compileShaderFromFile(vS,GLSLShader::FRAGMENT);
}