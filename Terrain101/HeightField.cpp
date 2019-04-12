#include "HeightField.h"

#include "glad/glad.h"
#include "GL/GL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


#include <iostream>
#include <fstream>

std::vector<Vertex> vertices;
std::vector<GLuint> indices;

//#define RENDERMODE_SWITCH

#ifdef RENDERMODE_SWITCH
  #define RENMODE_TRIANGLE_STRIP
#else
  #undef RENMODE_TRIANGLE_STRIP
#endif

int polling = 24;

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
// calculateCenterTransform
//---------------------------------------------------------------------
void HeightField::calculateCenterTransform()
{
  float oneOverRadius = 1.0f / _bndRadius;

  _mCentralizeTranslate = glm::translate(glm::mat4(1), -_vCen);

  if (oneOverRadius < 1.0f)
    _mUnitScale = glm::scale(glm::mat4(1), glm::vec3(oneOverRadius * 0.7));
  else
    _mUnitScale = glm::scale(glm::mat4(1), glm::vec3(_bndRadius * 1.4f));

  _mDefaultTransform = _mUnitScale * _mCentralizeTranslate;
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
      vertices.push_back(Vertex(glm::vec3(hMapX, hHeightField[hMapX][hMapZ], hMapZ), glm::vec2((float)hMapX/hX, (float)hMapZ/hZ)));
      bound(vertices.back().vtx);
    }
  }

#ifdef RENMODE_TRIANGLE_STRIP
  _renMode = GL_TRIANGLE_STRIP;

  for (int hMapX = 0; hMapX < hX - polling; hMapX+= polling)
  {
    for (int hMapZ = 0; hMapZ < hZ; hMapZ+= polling)
    {
      indices.push_back(hMapZ + (hMapX + polling)*hZ);

      if (hMapZ == 0 && hMapX != 0)
        indices.push_back(indices.back());

      indices.push_back(hMapX * hZ + hMapZ);

      if (hMapZ >= hZ - polling)
          indices.push_back(indices.back());
    }
  }
#else
  _renMode = GL_PATCHES;
    
  for (int hMapX = 0; hMapX < hX-polling; hMapX+= polling)
  {
    for (int hMapZ = 0; hMapZ < hZ-polling; hMapZ+= polling)
    {
       indices.push_back(hMapX * hZ + hMapZ);
       indices.push_back(hMapX * hZ + hMapZ + polling);
       indices.push_back((hMapX  + polling) * hZ + hMapZ + polling );
       indices.push_back((hMapX  + polling) * hZ + hMapZ );
    }
  }
#endif
  
  calculateCenterTransform();
  cacheToGPU();

  return true;
}

//---------------------------------------------------------------------
// cacheToGPU
//---------------------------------------------------------------------
void HeightField::cacheToGPU()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &IBO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size() , &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

#ifdef FIXED

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);

  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(3, GL_FLOAT, sizeof(Vertex), (void*) sizeof(glm::vec3));

#endif

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(glm::vec3));
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

}


//---------------------------------------------------------------------
// loadTexture
//---------------------------------------------------------------------
void HeightField::loadTexture(char *tFileName)
{
  int w, h, comp;
  unsigned char * image = stbi_load(tFileName, &w, &h, &comp, STBI_rgb);

  glGenTextures(1, &tID);
  glBindTexture(GL_TEXTURE_2D, tID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if(comp == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  else if(comp == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

  glBindTexture(GL_TEXTURE_2D,0);
    
  stbi_image_free(image);
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

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glDrawElements(_renMode, indices.size(), GL_UNSIGNED_INT, (void*)0);
}

//---------------------------------------------------------------------
// render
//---------------------------------------------------------------------
void HeightField::render(Shader *prog, glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot, int outer, int inner)
{
  glm::mat4 mMVP = proj * view * rot * _mDefaultTransform;

  prog->use();
  prog->setUniform("mMVP", mMVP);

  prog->setUniform("Outer", outer);
  prog->setUniform("Inner", inner);
  
  glBindTexture(GL_TEXTURE_2D, tID);
  glBindVertexArray(VAO);
  glPatchParameteri(GL_PATCH_VERTICES,4);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glDrawElements(_renMode, indices.size(), GL_UNSIGNED_INT, (void*)0);
  
}

