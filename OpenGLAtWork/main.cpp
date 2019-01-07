#include<iostream>

#include "glm/glm.hpp"
//#include "glad/glad.h"

#include "GLFW/glfw3.h"

int main(void)
{
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  //{
  //  std::cout << "Failed to initialize OpenGL context" << std::endl;
  //  return -1;
  //}

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    glPointSize(8.0f);

    glBegin(GL_POINTS);

    glVertex2d(0,0);

    glEnd();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}