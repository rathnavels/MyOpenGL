#include <iostream>
#include <fstream>
#include <algorithm>

#include "glad/glad.h"
#include "GL/GL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp> 

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "HeightField.h"

std::vector<Vertex3>    vertices3;
std::vector<GLuint>     indices3;
std::vector<Vertex2>    vertices2;
std::vector<GLuint>     indices2;

#define COMPLETE_GPU_LOD

#define PATCHSIZE 64

int polling = 24;

//---------------------------------------------------------------------
// determine3
//---------------------------------------------------------------------
void HeightField::determine3(void)
{
  _vCen3 = (_vMax3 + _vMin3) * 0.5f;
  _bndRadius = glm::distance(_vMax3, _vMin3) * 0.5f;
}

//---------------------------------------------------------------------
// determine2
//---------------------------------------------------------------------
void HeightField::determine2(void)
{
  _vCen2 = (_vMax2 + _vMin2) * 0.5f;
  _bndRadius = glm::distance(_vMax2, _vMin2) * 0.5f;
}


//---------------------------------------------------------------------
// bound3
//---------------------------------------------------------------------
void  HeightField::bound(const glm::vec3 &v)
{
  _vMin3 = glm::min(_vMin3, v);
  _vMax3 = glm::max(_vMax3, v);

  determine3();
}

//---------------------------------------------------------------------
// bound2
//---------------------------------------------------------------------
void  HeightField::bound(const glm::vec2 &v)
{
  _vMin2 = glm::min(_vMin2, v);
  _vMax2 = glm::max(_vMax2, v);

  determine2();
}

//---------------------------------------------------------------------
// calculateCenterTransform
//---------------------------------------------------------------------
void HeightField::calculateCenterTransform(glm::vec3 cen)
{
  float oneOverRadius = 1.0f / (rows * 0.5 * _gridSpacing);

  _mCentralizeTranslate = glm::translate(glm::mat4(1), -glm::vec3(rows * cen.x * _gridSpacing , 0.0f, cols * cen.z * _gridSpacing));

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
// createCompGPU
//---------------------------------------------------------------------
bool HeightField::createCompGPU(char *hFileName)
{
  cv::Mat dem = cv::imread(hFileName, cv::IMREAD_LOAD_GDAL | cv::IMREAD_ANYDEPTH);
  rows = dem.rows;
  cols = dem.cols;
  int bd = dem.depth();

  minHeight = FLT_MAX;  
  maxHeight = -FLT_MAX;

  float *hLoad = new float[dem.rows * dem.cols];

  for(int y=0; y<rows; y++)
  {
    for(int x=0; x<cols; x++)
    { 
        float h                  = dem.at<float>(cv::Point(x, y));
        hLoad[(y  * cols + x )]  = h;
        minHeight                = std::min(minHeight,h);
        maxHeight                = std::max(maxHeight,h);
    }
  }

  int nopX = (float)cols / PATCHSIZE;
  int nopZ = (float)rows / PATCHSIZE;

  glGenTextures(1, &heightMaptID);
  glBindTexture(GL_TEXTURE_2D, heightMaptID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, cols, rows, 0, GL_RED, GL_FLOAT, hLoad);

// check OpenGL error
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }

  glBindTexture(GL_TEXTURE_2D,0);

  _scaleFactor = 1;

  for(int z = 0; z < nopZ; z++)
    for(int x = 0; x < nopX; x++)
    {
      vertices2.push_back(Vertex2(glm::vec2((float)x * PATCHSIZE/cols, (float)z * PATCHSIZE/rows)));
      bound(vertices2.back().vtx);
    }

  //calculateCenterTransform(glm::vec3(_vCen2.x , 0.0f, _vCen2.y));
  
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
// loadTexture
//---------------------------------------------------------------------
void HeightField::loadTexture(char *tFileName)
{
cv::Mat img         = cv::imread(tFileName);
int w               = img.cols;
int h               = img.rows;
int comp            = img.channels();

  glGenTextures(1, &tID);
  glBindTexture(GL_TEXTURE_2D, tID);

  if(comp == 3)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, h, w, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
  else if(comp == 4)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, h, w, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D,0);

  img.deallocate();
}

//---------------------------------------------------------------------
// render
//---------------------------------------------------------------------
void HeightField::render(Shader *prog, glm::mat4 &view, glm::mat4 &proj, glm::mat4 &rot, glm::mat3 &tex)
{
  glm::mat4 mMVP = proj * view * rot * _mDefaultTransform;

  prog->use();

  prog->setUniform("heightStep",      _heightStep);
  prog->setUniform("gridSpacing",     _gridSpacing);
  prog->setUniform("scaleFactor",     1);

  calculateCenterTransform(glm::vec3(_vCen2.x , 0.0, _vCen2.y));
  
  prog->setUniform("normalMatrix",glm::transpose(glm::inverse(view * rot * _mDefaultTransform)));
  prog->setUniform("mMVP", mMVP);
  //prog->setUniform("mTex", tex);

  prog->setUniform("moveY",(minHeight+maxHeight)/2.0f);

 // prog->setSamplerUniform("uTexture0", 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tID);

  prog->setSamplerUniform("heightMap", 1);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, heightMaptID);

  glBindVertexArray(heightMapPatchVAO);
  glPatchParameteri(GL_PATCH_VERTICES,1);

  glDrawArrays(GL_PATCHES, 0, vertices2.size());  
}

