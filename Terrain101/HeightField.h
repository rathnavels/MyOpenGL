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

  unsigned int heightMaptID;
  unsigned int heightMapPatchVAO;
  unsigned int heightMapPatchVBO;
  unsigned int heightMapPatchIBO;

public:

  bool createCompGPU(char *hFileName);

  void loadTexture(char *tFileName);

  void render(Shader *prog, glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot, int outerTess, int innerTess);

  void render(Shader *prog, glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot);

  void bound(const glm::vec3 &v);

  void bound(const glm::vec2 &v);

  void determine3();

  void determine2();

  void calculateCenterTransform(glm::vec3 cen);

  void cacheToGPU();

  glm::vec3           _vMin3;                            ///<    Min Vertex
  glm::vec3           _vMax3;                            ///<    Max Vertex
  glm::vec3           _vCen3;                            ///<    VertexLst center vertex

  glm::vec2           _vMin2;                            ///<    Min Vertex
  glm::vec2           _vMax2;                            ///<    Max Vertex
  glm::vec2           _vCen2;                            ///<    VertexLst center vertex

  float               _bndRadius;                        ///<    Bounding Radius

  GLuint              _renMode;

  glm::mat4           _mCentralizeTranslate;
  glm::mat4           _mUnitScale;
  glm::mat4           _mDefaultTransform;

  //BYTE hHeightField[1024][1024];

  //unsigned short hHF[1024][1024];
  short hLoad[1346][1484];

  int                 _scaleFactor;
  float               _heightStep;
  float               _gridSpacing;
  
  HeightField(void) : _vMin3(FLT_MAX),
                      _vMax3(-FLT_MAX),
                      _vCen3(0),
                      _vMin2(FLT_MAX),
                      _vMax2(-FLT_MAX),
                      _vCen2(0),
                      _bndRadius(0),
                      _mCentralizeTranslate(glm::mat4(1)),
                      _mUnitScale(glm::mat4(1)),
                      _mDefaultTransform(glm::mat4(1)),
                      _gridSpacing(1),
                      _heightStep(1)
                    {}
 
};

//---------------------------------------------------------------------
// VertexClass
//---------------------------------------------------------------------
class Vertex3
{
public:
  glm::vec3 vtx;
  glm::vec2 tC;

  Vertex3(glm::vec3 v, glm::vec2 tc)
  {
    vtx = v;
    tC = tc;
  }
};


//---------------------------------------------------------------------
// VertexClass
//---------------------------------------------------------------------
class Vertex2
{
public:
  glm::vec2 vtx;
  Vertex2(glm::vec2 v)
  {
    vtx = v;
  }
};

