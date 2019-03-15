#include<iostream>

#include <glm\glm.hpp>

namespace GLSLShader
{
  enum GLSLShaderType
  {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    TESS_CONTROL,
    TESS_EVAL
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

    void bindAttribLocation(uint16_t location, const char* name);
    void bindFragDataLocation(uint16_t location, const char *name );

    void setUniform(const char* name, float x, float y, float z);

    void setUniform(const char* name, const glm::vec3 &v);
    void setUniform(const char* name, const glm::vec4 &v);

    void setUniform(const char* name, const glm::mat4 &m);
    void setUniform(const char* name, const glm::mat3 &m);

    void setUniform(const char* name, float val);
    void setUniform(const char* name, int val);

    void setUniform(const char* name, bool val);
    
};
