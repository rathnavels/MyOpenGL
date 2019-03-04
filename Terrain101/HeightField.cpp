#include "HeightField.h"

#include "glad/glad.h"
#include "GL/GL.h"
#include "stb/stb_image.h"


#include <iostream>
#include <vector>

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

std::vector<Vertex> vertices;

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
      vertices.push_back(Vertex(glm::vec3(hMapX, hHeightField[hMapX][hMapZ], hMapZ), glm::vec3((float)hHeightField[hMapX][hMapZ]/250, 0.2, 0.6)));
      bound(vertices.back().vtx);

      //vertices.push_back(Vertex(glm::vec3(hMapX, hHeightField[hMapX][hMapZ], hMapZ),glm::vec3((float)hHeightField[hMapX][hMapZ] / 250,0.2,0.6)));
      //bound(vertices.back().vtx);
      //vertices.push_back(Vertex(glm::vec3(hMapX, hHeightField[hMapX][hMapZ + 1], hMapZ + 1), glm::vec3((float)hHeightField[hMapX][hMapZ+1] / 250, 0.2, 0.6)));
      //bound(vertices.back().vtx);
      //vertices.push_back(Vertex(glm::vec3(hMapX + 1, hHeightField[hMapX+1][hMapZ], hMapZ), glm::vec3((float)hHeightField[hMapX+1][hMapZ] / 250, 0.2, 0.6)));
      //bound(vertices.back().vtx);
      //vertices.push_back(Vertex(glm::vec3(hMapX + 1, hHeightField[hMapX+1][hMapZ+1], hMapZ+1), glm::vec3((float)hHeightField[hMapX+1][hMapZ+1] / 250, 0.2, 0.6)));
    }
  }

  float oneOverRadius = 1.0f / _bndRadius;


  _mCentralizeTranslate = glm::translate(glm::mat4(1), -_vCen);

  if (oneOverRadius < 1.0f)
    _mUnitScale = glm::scale(glm::mat4(1), glm::vec3(oneOverRadius * 0.7));
  else
    _mUnitScale = glm::scale(glm::mat4(1), glm::vec3(_bndRadius * 1.4f));

  _mDefaultTransform = _mUnitScale * _mCentralizeTranslate;

  glPointSize(1.0);
  //glDepthFunc(GL_LEQUAL);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);


  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size() , &vertices[0], GL_STATIC_DRAW);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);

  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_FLOAT, sizeof(Vertex), (void*) sizeof(glm::vec3));

  //glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);

  //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glTexCoordPointer(2, GL_FLOAT, sizeof(glm::vec2), 0);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  return true;
}


//---------------------------------------------------------------------
// loadTexture
//---------------------------------------------------------------------
void HeightField::loadTexture(char *tFileName)
{
  std::string fPath;
  char filePath[MAX_PATH];
  
  _getcwd(filePath, FILENAME_MAX);
  fPath = std::string(filePath) + "/" + std::string(tFileName);

  int w,h,n;
  //stbi_uc *imgData = stbi_load(fPath.c_str(), &w, &h, &n, 3);

  //glEnable(GL_TEXTURE_2D);  
  //glBindTexture(GL_TEXTURE_2D, tID);


  //glDisable(GL_TEXTURE_2D);
    
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

  glEnable(GL_COLOR);
  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, vertices.size());


}

