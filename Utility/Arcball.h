// Arcball.h
// Rathinavel Sankaralingam
// February 21, 2017

#pragma once

//---------------------------------------------------------------------------------

//Includes
//System

//3rd party libs
#include <glm\glm.hpp>

//ObjGL


//---------------------------------------------------------------------------------
//Classes
class Arcball
{
  //Define
private:
protected:
public:

  //Members
private:
  glm::vec2           _center;
  float               _radius;
  glm::vec3           _pointOnSphere;
  bool                _XY;

protected:
public:

  //Methods
private:
protected:
public:
  glm::vec3 _plane2sphere(glm::vec2&);
  glm::quat update(glm::vec2& dimensions, float nx, float ny, glm::vec3 &axis, float &angle);
  glm::quat update(glm::vec2& dimensions, float nx, float ny);

  Arcball(glm::vec2& dimensions, float ox, float oy, bool XY = false);
  Arcball(void) {}
  ~Arcball() {}

};



