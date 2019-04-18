#pragma once

#include<iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.h"

namespace GLSLShader
{
  enum GLSLShaderType
  {
    VERTEX  = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    TESS_CONTROL = GL_TESS_CONTROL_SHADER,
    TESS_EVAL = GL_TESS_EVALUATION_SHADER
  };
};


class Shader
{
  private:
    int handle;
    bool linked;
    std::string logString;

    int getUniformLocation(const char *name);
    bool fileExists(const std::string &filename);

  public:
    bool compileShaderFromFile(const char *filename, GLSLShader::GLSLShaderType type);

    bool link();

    void use();
    
    int getHandle();

    bool isLinked();

    void setUniform(const char* name, float x, float y, float z);

    void setUniform(const char* name, const glm::vec3 &v);

    void setUniform(const char* name, const glm::vec4 &v);

    void setUniform(const char* name, const glm::mat4 &m);

    void setUniform(const char* name, const glm::mat3 &m);

    void setUniform(const char* name, float val);

    void setUniform(const char* name, int val);

    void setUniform(const char* name, bool val);

    void bindAttribLocation(uint16_t location, const char* name);

    void bindFragDataLocation(uint16_t location, const char *name );

    void setSamplerUniform(const char* name, int val);

    Shader(const char *vS, const char *fS);

    Shader(void){}

    ~Shader(){}
    
};
