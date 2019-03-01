#include "HeightField.h"

#include "glad/glad.h"
#include "GL/GL.h"


#include <iostream>
#include <vector>


std::vector<glm::vec3> vertices;

//---------------------------------------------------------------------
// bound
//---------------------------------------------------------------------
void HeightField::determine(void)
{
  _vCen = (_vMax + _vMin) * 0.5f;
  _vFW = (_vMax - _vMin);
  _bndRadius = glm::distance(_vMax, _vMin) * 0.5f;
}

//---------------------------------------------------------------------
// bound
//---------------------------------------------------------------------
void  HeightField::bound(const glm::vec3 &v)
{
  _vMin = glm::min(_vMin, v);
  _vMax = glm::max(_vMax, v);

  determine();
}


//---------------------------------------------------------------------
// create
//---------------------------------------------------------------------
bool HeightField::create(char *hFileName, int hX, int hZ)
{
  FILE *fp;
  fopen_s(&fp, hFileName, "rb");
  fread(hHeightField, 1, hX * hZ, fp);
  fclose(fp);

  hmX = hX;
  hmZ = hZ;

  for (int hMapX = 0; hMapX < hX; hMapX++) 
  {
    for (int hMapZ = 0; hMapZ < hZ; hMapZ++) 
    {
      vertices.push_back(glm::vec3(hMapX, hHeightField[hMapX][hMapZ], hMapZ));
      bound(vertices.back());
    }
  }

  float oneOverRadius = 1.0f / _bndRadius;


  _mCentralizeTranslate = glm::translate(glm::mat4(1), -_vCen);

  //if (oneOverRadius < 1.0f)
  //  _mUnitScale = glm::scale(glm::mat4(), glm::vec3(oneOverRadius * 0.7));
  //else
  //  _mUnitScale = glm::scale(glm::mat4(), glm::vec3(_bndRadius * 1.4f));

  _mDefaultTransform = _mUnitScale * _mCentralizeTranslate;

  glPointSize(2.0);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);


  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(glm::vec3), 0);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  return true;
}


//---------------------------------------------------------------------
// render
//---------------------------------------------------------------------
void HeightField::render(glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot)
{

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(proj));


  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(glm::value_ptr( view * rot * _mDefaultTransform));

  
  glBindVertexArray(VAO);
  glColor3f(0.0,0.8,0.0);
  glDrawArrays(GL_POINTS, 0, vertices.size());


}

