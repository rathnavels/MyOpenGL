#include "HeightField.h"

#include "glad/glad.h"
#include "GL/GL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp> 

#include <iostream>
#include <fstream>

std::vector<Vertex3>    vertices3;
std::vector<GLuint>     indices3;
std::vector<Vertex2>    vertices2;
std::vector<GLuint>     indices2;

#define COMPLETE_GPU_LOD

#define PATCHSIZE 64

int polling = 24;

//---------------------------------------------------------------------
// bound
//---------------------------------------------------------------------
void HeightField::determine3(void)
{
  _vCen3 = (_vMax3 + _vMin3) * 0.5f;
  _bndRadius = glm::distance(_vMax3, _vMin3) * 0.5f;
}

//---------------------------------------------------------------------
// bound
//---------------------------------------------------------------------
void  HeightField::bound(const glm::vec3 &v)
{
  _vMin3 = glm::min(_vMin3, v);
  _vMax3 = glm::max(_vMax3, v);

  determine3();
}

//---------------------------------------------------------------------
// calculateCenterTransform
//---------------------------------------------------------------------
void HeightField::calculateCenterTransform()
{
  float oneOverRadius = 1.0f / _bndRadius;

  _mCentralizeTranslate = glm::translate(glm::mat4(1), -_vCen3);

  if (oneOverRadius < 1.0f)
    _mUnitScale = glm::scale(glm::mat4(1), glm::vec3(oneOverRadius * 0.7));
  else
    _mUnitScale = glm::scale(glm::mat4(1), glm::vec3(_bndRadius * 1.4f));

  _mDefaultTransform = _mUnitScale * _mCentralizeTranslate;
}

//---------------------------------------------------------------------
// transformTexCoord
//---------------------------------------------------------------------
inline glm::vec2 transformTexCoord(glm::mat3 tT, glm::vec2 tc)
{
  tc.x = tc.x * tT[0][0] + tT[2][0];
  tc.y = tc.y * tT[1][1] + tT[2][1];

  return tc;
}

//---------------------------------------------------------------------
// create
//---------------------------------------------------------------------
bool HeightField::createBasic(char *hFileName, int hX, int hZ)
{
  FILE *fp;
  fopen_s(&fp, hFileName, "rb");

  if(fp==NULL)
    return false;

  fread(hHeightField, 1, hX * hZ, fp);
  fclose(fp);

  hmX = hX;
  hmZ = hZ;

  for (int hMapX = 0; hMapX < hX; hMapX++) 
  {
    for (int hMapZ = 0; hMapZ < hZ; hMapZ++) 
    {
      vertices3.push_back(Vertex3(glm::vec3(hMapX, hHeightField[hMapX][hMapZ], hMapZ), glm::vec2((float)hMapX/hX, (float)hMapZ/hZ)));
      bound(vertices3.back().vtx);
    }
  }

  _renMode = GL_PATCHES;
    
  for (int hMapX = 0; hMapX < hX-polling; hMapX+= polling)
  {
    for (int hMapZ = 0; hMapZ < hZ-polling; hMapZ+= polling)
    {
       indices3.push_back(hMapX * hZ + hMapZ);
       indices3.push_back(hMapX * hZ + hMapZ + polling);
       indices3.push_back((hMapX  + polling) * hZ + hMapZ + polling );
       indices3.push_back((hMapX  + polling) * hZ + hMapZ );
    }
  }
  
  calculateCenterTransform();
  cacheToGPU();

  return true;
}

//---------------------------------------------------------------------
// createCompGPU
//---------------------------------------------------------------------
bool HeightField::createCompGPU(char *hFileName, int hX, int hZ)
{
  FILE *fp;

  //unsigned short hHF[1048576];
  //unsigned char hLoad[1048576];

  fopen_s(&fp, hFileName, "rb");

  if(fp==NULL)
    return false;

  fread(hLoad, 1, hX * hZ, fp);
  fclose(fp);

  hmX = hX;
  hmZ = hZ;


  //hHF = (unsigned short*)malloc(sizeof(unsigned short)*hX*hZ);
  //memcpy(hHF,(uint8_t*)hLoad,sizeof(unsigned char)*hX*hZ);

  for(int i=0; i<hX; i++)
    for(int j=0; j<hZ; j++)
      hHF[(i*hZ)+j] = hLoad[(i*hZ)+j];

  glGenTextures(1, &heightMaptID);
  glBindTexture(GL_TEXTURE_2D, heightMaptID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, hX, hZ, 0, GL_RED, GL_UNSIGNED_SHORT, &hHF[0]);

  glBindTexture(GL_TEXTURE_2D,0);

  int nopX = hX / PATCHSIZE;
  int nopZ = hZ / PATCHSIZE;
  int numPatches = nopX * nopZ;

  _scaleFactor = 1;

  for (int x = 0; x < nopX; x++) 
    for (int z = 0; z < nopX; z++) 
       vertices2.push_back(Vertex2(glm::vec2((float)x * PATCHSIZE/hX, (float)z * PATCHSIZE/hZ)));


 // _mDefaultTransform = glm::translate(glm::mat4(1), glm::vec3(hX * 0.5, 0.0, hZ * 0.5));
  
  glGenVertexArrays(1, &heightMapPatchVAO);
  glBindVertexArray(heightMapPatchVAO);

  glGenBuffers(1,&heightMapPatchVBO);
  glBindBuffer(GL_ARRAY_BUFFER, heightMapPatchVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2) * vertices2.size(), &vertices2[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), 0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

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
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3) * vertices3.size() , &vertices3[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices3.size(), &indices3[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*) sizeof(glm::vec3));
  
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
  glDrawElements(GL_PATCHES, indices3.size(), GL_UNSIGNED_INT, (void*)0);
  
}

//---------------------------------------------------------------------
// render
//---------------------------------------------------------------------
void HeightField::render(Shader *prog, glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot)
{
  glm::mat4 mMVP = proj * view * rot * _mDefaultTransform;

  prog->use();
  prog->setUniform("mMVP", mMVP);
  prog->setUniform("normalMatrix",glm::transpose(glm::inverse(view * rot*_mDefaultTransform)));

  prog->setUniform("heightStep",2.0f);
  prog->setUniform("gridSpacing",1.0f);
  prog->setUniform("scaleFactor",_scaleFactor);

  prog->setSamplerUniform("texUnit", 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tID);

  prog->setSamplerUniform("heightMap", 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, heightMaptID);

#ifndef COMPLETE_GPU_LOD
  glBindVertexArray(VAO);
  glPatchParameteri(GL_PATCH_VERTICES,4);
#else
  glBindVertexArray(heightMapPatchVAO);
  glPatchParameteri(GL_PATCH_VERTICES,1);
#endif

  glDrawArrays(GL_PATCHES, 0, vertices2.size());
  
}

