//----COPYRIGHT NOTICE--------------------------------------------------
//
//	Copyright (C) FoVi3D
//
//----COPYRIGHT NOTICE--------------------------------------------------

// Arcball.cpp
// Rathinavel Sankaralingam
// February 21, 2018


//---------------------------------------------------------------------
// Includes
// System

// ObjGL
#include "Arcball.h"

// 3rdPartyLibs
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
//---------------------------------------------------------------------



//---------------------------------------------------------------------
// Arcball Constructor
/// \brief Default constructor
/// \param dimensions arc ball dimensions
/// \param ox mouse X position
/// \param oy mouse Y position
/// \param XY default false
//---------------------------------------------------------------------
Arcball::Arcball(glm::vec2& dimensions, float ox, float oy, bool XY)
{
  glm::vec2 p(ox, oy);

  _radius = (dimensions.x > dimensions.y) ? dimensions.x / 2.0f : dimensions.y / 2.0f;
  _center = glm::vec2(dimensions.x / 2, dimensions.y / 2);
  _XY = XY;

  _pointOnSphere = _plane2sphere(p);
}

//---------------------------------------------------------------------
// Arcball PointOnSphere
/// \brief point on sphere
/// \param v xy on sphere
/// \return radius
//---------------------------------------------------------------------
glm::vec3 Arcball::_plane2sphere(glm::vec2 &v)
{
  glm::vec2         f = v;
  glm::vec3         r;
  double            lenSquared;
  double            len;

  f /= _radius;
  lenSquared = f.x * f.x + f.y * f.y;
  len = sqrt(lenSquared);

  if (lenSquared > 1.0)
  {
    if (!_XY)
      r = glm::vec3(f.x / len, 0.0f, f.y / len);
    else
      r = glm::vec3(f.x / len, f.y / len, 0.0f);
  }
  else
  {
    double fz = sqrt(1 - lenSquared);
    if (!_XY)
      r = glm::vec3(f.x, fz, f.y);
    else
      r = glm::vec3(f.x, f.y, fz);
  }

  return r;
}

//---------------------------------------------------------------------
// Arcball Update
/// \brief update position
/// \param dimensions xy on sphere
/// \param ox update x position
/// \param oy update y position
/// \return quaternion rotation
//---------------------------------------------------------------------
glm::quat Arcball::update(glm::vec2& dimensions, float ox, float oy)
{
  glm::vec3                 position;
  glm::vec3                 vCp;
  glm::quat                 qRot;

  position = _plane2sphere(glm::vec2(ox, oy));
  vCp = glm::cross(_pointOnSphere, position);
  qRot = glm::quat(glm::dot(_pointOnSphere, position), vCp.x, vCp.y, vCp.z);

  _pointOnSphere = position;

  return qRot;
}


//---------------------------------------------------------------------
// Arcball Update
/// \brief update position
/// \param dimensions xy on sphere
/// \param ox update x position
/// \param oy update y position
/// \param axis referencing cross product computation of point to sphere and plane to sphere
/// \param angle angle of rotation reference
/// \return quaternion rotation
//---------------------------------------------------------------------
glm::quat Arcball::update(glm::vec2& dimensions, float ox, float oy, glm::vec3 &axis, float &angle)
{
  glm::vec3                 position;
  glm::vec3                 vCp;
  glm::quat                 qRot;

  position = _plane2sphere(glm::vec2(ox, oy));

  vCp = glm::cross(_pointOnSphere, position);
  axis = vCp;
  angle = acos(glm::min(1.0f, glm::dot(_pointOnSphere, position)));
  qRot = glm::quat(glm::dot(_pointOnSphere, position), vCp.x, vCp.y, vCp.z);

  _pointOnSphere = position;

  return qRot;
}

