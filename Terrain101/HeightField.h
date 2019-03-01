#pragma once

#include <Windows.h>

#define GL_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/ext.hpp>



class HeightField
{
private:
  int hmX, hmZ;
  unsigned int VBO, VAO;

public:

  bool create(char *hFileName, int hX, int hZ);

  void render(glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot);

  glm::vec3           _vMin;                            ///<    Min Vertex
  glm::vec3           _vMax;                            ///<    Max Vertex
  glm::vec3           _vCen;                            ///<    VertexLst center vertex
  glm::vec3           _vFW;                             ///<    View volume full width
  float               _bndRadius;                       ///<    Bounding Radius

  glm::mat4           _mCentralizeTranslate;
  glm::mat4           _mUnitScale;
  glm::mat4           _mDefaultTransform;

  void bound(const glm::vec3 &v);
  void determine();

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

