#pragma once

#include <Windows.h>
#include <direct.h>
#include <fstream>
#include <vector>

#define GL_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Shader.h"


//---------------------------------------------------------------------
// HeightField class
//---------------------------------------------------------------------
class HeightField
{
private:
  int hmX;
  int hmZ;
  unsigned int VBO;
  unsigned int VAO;
  unsigned int IBO;
  unsigned int tID;

public:

  bool create(char *hFileName, int hX, int hZ);

  void loadTexture(char *tFileName);

  void render(glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot);

  void render(Shader *prog, glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot);

  void bound(const glm::vec3 &v);

  void determine();

  void calculateCenterTransform();

  void cacheToGPU();

  glm::vec3           _vMin;                            ///<    Min Vertex
  glm::vec3           _vMax;                            ///<    Max Vertex
  glm::vec3           _vCen;                            ///<    VertexLst center vertex
  glm::vec3           _vFW;                             ///<    View volume full width
  float               _bndRadius;                       ///<    Bounding Radius

  GLuint              _renMode;

  glm::mat4           _mCentralizeTranslate;
  glm::mat4           _mUnitScale;
  glm::mat4           _mDefaultTransform;

  BYTE hHeightField[1024][1024];

  HeightField(void) : _vMin(FLT_MAX),
                      _vMax(-FLT_MAX),
                      _vCen(0),
                      _vFW(0),
                      _bndRadius(0),
                      _mCentralizeTranslate(glm::mat4(1)),
                      _mUnitScale(glm::mat4(1)),
                      _mDefaultTransform(glm::mat4(1))
                    {}
 
};

//---------------------------------------------------------------------
// VertexClass
//---------------------------------------------------------------------
class Vertex
{
public:
  glm::vec3 vtx;
  glm::vec3 clr;

  Vertex(glm::vec3 v, glm::vec3 c)
  {
    vtx = v;
    clr = c;
  }
};

