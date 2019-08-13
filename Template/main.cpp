//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <iostream>
//#include <ctime>
//#include <string>
//#include <sstream>
//
//#include <pcl/io/pcd_io.h>
//#include <pcl/point_types.h>
//
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/rotate_vector.hpp>
//
//#include "Shader.h"
//#include "Arcball.h"
//
//#define  STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb/stb_image_write.h"
//
//#include <anttweakbar.h>
//
//
//
//// settings
//unsigned int SCR_WIDTH = 1280;
//unsigned int SCR_HEIGHT = 1280;
//
//glm::mat4 viewMat, projMat, modelmat;
//
//Arcball          arcball;
//Shader          *pointCloudShader = new Shader();
//
//bool             LEFT = false;
//bool             RIGHT = false;
//
//
////---------------------------------------------------------------------
//// setupCamera
////---------------------------------------------------------------------
//static void setupCamera()
//{
//	glm::vec3 lookFrom, lookAt, up;
//
//	lookFrom = glm::vec3(0, 0, 3);
//	lookAt = glm::vec3(0, 0, 0);
//	up = glm::vec3(0, 1, 0);
//
//	viewMat = glm::lookAt(lookFrom, lookAt, up);
//	projMat = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 500.0f);
//}
//
//
////---------------------------------------------------------------------
//// Resize_Callback
////---------------------------------------------------------------------
//static void Resize_Callback(GLFWwindow *pW, int w, int h)
//{
//	float aR = (float)w / (float)h;
//
//	glViewport(0, 0, w, h);
//	SCR_WIDTH = w;
//	SCR_HEIGHT = h;
//
//	projMat = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 6000.0f);
//}
//
////---------------------------------------------------------------------
//// initCompGPU
////---------------------------------------------------------------------
//bool initCompGPU()
//{
//	pointCloudShader->compileShaderFromFile("glsl/pclShader.ver", GLSLShader::GLSLShaderType::VERTEX);
//	pointCloudShader->compileShaderFromFile("glsl/pclShader.tes", GLSLShader::GLSLShaderType::TESS_EVAL);
//
//	pointCloudShader->link();
//	pointCloudShader->use();
//
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glFrontFace(GL_CCW);
//
//	return true;
//}
//
////---------------------------------------------------------------------
//// MouseButton_Callback
////---------------------------------------------------------------------
//static void MouseButton_Callback(GLFWwindow *pW, int button, int action, int mods)
//{
//
//}
//
//
////---------------------------------------------------------------------
//// MouseMotion_Callback
////---------------------------------------------------------------------
//static void MouseMotion_Callback(GLFWwindow *pW, double x, double y)
//{
//
//}
//
////---------------------------------------------------------------------
//// MouseScroll_Callback
////---------------------------------------------------------------------
//static void MouseScroll_Callback(GLFWwindow *pW, double x, double y)
//{
//
//}
//
//
////---------------------------------------------------------------------
//// Keyboard_Callback
////---------------------------------------------------------------------
//static void Keyboard_Callback(GLFWwindow *pW, int key, int scancode, int action, int mods)
//{
//
//}
//
////---------------------------------------------------------------------
//// Drop_Callback
////---------------------------------------------------------------------
//void Drop_Callback(GLFWwindow *pW, int count, const char** paths)
//{
//
//}
//
////---------------------------------------------------------------------
//// display
////---------------------------------------------------------------------
//void display(GLFWwindow *pWindow)
//{
//	while (!glfwWindowShouldClose(pWindow))
//	{
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
//
//		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//		glFlush();
//
//		glfwSwapBuffers(pWindow);
//		glfwPollEvents();
//	}
//}
//
////---------------------------------------------------------------------
//// glInitWindow
////---------------------------------------------------------------------
//GLFWwindow* glInitWindow(const int &X, const int &Y, char *name)
//{
//	GLFWwindow *pW = 0;
//
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//	glfwWindowHint(GLFW_DEPTH_BITS, 32);
//	glfwWindowHint(GLFW_STENCIL_BITS, 8);
//	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
//
//	pW = glfwCreateWindow(X, Y, name, 0, 0);
//
//	if (pW)
//	{
//		glfwMakeContextCurrent(pW);
//		glfwSwapInterval(1);
//		glfwShowWindow(pW);
//	}
//
//	return pW;
//}
//
//
//
////---------------------------------------------------------------------
//// main
////---------------------------------------------------------------------
//int main()
//{
//	GLFWwindow              *pWindow = 0;
//
//	if (glfwInit())
//	{
//		pWindow = glInitWindow(SCR_WIDTH, SCR_HEIGHT, "Terrain101");
//
//		if (pWindow)
//		{
//			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//			{
//				std::cout << "Failed to initialize GLAD" << std::endl;
//				return -1;
//			}
//
//			glfwSetKeyCallback(pWindow, Keyboard_Callback);
//			glfwSetMouseButtonCallback(pWindow, MouseButton_Callback);
//			glfwSetCursorPosCallback(pWindow, MouseMotion_Callback);
//			glfwSetScrollCallback(pWindow, MouseScroll_Callback);
//			glfwSetWindowSizeCallback(pWindow, Resize_Callback);
//			glfwSetDropCallback(pWindow, Drop_Callback);
//
//			//if (initCompGPU())
//			if (1)
//			{
//				setupCamera();
//				display(pWindow);
//			}
//
//			glfwDestroyWindow(pWindow);
//		}
//		glfwTerminate();
//	}
//
//	return 0;
//}
//


void main()
{

}
