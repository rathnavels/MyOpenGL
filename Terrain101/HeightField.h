#pragma once

#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>



class HeightField
{
private:
  int hmX, hmZ;
  unsigned int VBO, VAO;

public:

  bool create(char *hFileName, int hX, int hZ);

  void render(glm::mat4 &view, glm::mat4 &proj);

  BYTE hHeightField[1024][1024];
 
};

