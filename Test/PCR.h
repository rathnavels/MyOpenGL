#pragma once


#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

#define GL_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Shader.h"

using namespace std;

class PCR
{
	public:

		int width;
		int height;

		unsigned int VBO;
		unsigned int VAO;
		unsigned int IBO;


		void loadPointsToGPU(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud);
		void render();
		PCR();
};