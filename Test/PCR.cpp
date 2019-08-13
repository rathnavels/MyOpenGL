#include "glad/glad.h"
#include "GL/GL.h"

#include <future>

#include "PCR.h"


void PCR::loadPointsToGPU(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud)
{
	// Set the initial minimum value of each coordinate:
	float min_x{ 0 }, max_x{ 0 }, min_y{ 0 }, max_y{ 0 }, min_z{ 0 }, max_z{ 0 };


	min_x = cloud->points[0].x;
	min_y = cloud->points[0].y;
	min_z = cloud->points[0].z;

	// Calculate the minimum value of each coordinate:
	for (size_t i = 0; i < cloud->points.size(); ++i)
	{
		if (cloud->points[i].x < min_x) min_x = cloud->points[i].x;
		if (cloud->points[i].y < min_y) min_y = cloud->points[i].y;
		if (cloud->points[i].z < min_z) min_z = cloud->points[i].z;
	}

	// Transform the cloud to the origin of its coordinate system, for easier handling of the cloud data.
	// This part of the code should be removed later. (Just helped me at the beginning)
	for (size_t i = 0; i < cloud->points.size(); ++i)
	{
		cloud->points[i].x -= min_x;
		cloud->points[i].y -= min_y;
		cloud->points[i].z -= min_z;
	}

	std::size_t v_size = cloud->points.size() * 3; // size of my points

	std::vector<GLfloat> vertices(v_size);

	// for efficient data reading, I start a new thread for reading the half of my data.
	std::future<void> result(std::async([&]()
	{
		for (size_t i = 1; i < cloud->points.size(); i += 2)
		{
			size_t num = (i * 6);

			vertices[num + 0] = cloud->points[i].x;
			vertices[num + 1] = cloud->points[i].y;
			vertices[num + 2] = cloud->points[i].z;
		}
	}));

	// another half of my points
	for (size_t i = 0; i < cloud->points.size(); i += 2)
	{
		size_t num = (i * 6);

		vertices[num + 0] = cloud->points[i].x;
		vertices[num + 1] = cloud->points[i].y;
		vertices[num + 2] = cloud->points[i].z;
	}

	result.get();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
}

void PCR::render()
{

}

PCR::PCR()
{

}