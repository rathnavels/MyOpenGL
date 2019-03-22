#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <ctime>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "HeightField.h"
#include "Arcball.h"
#include "Shader.h"

#define  STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// settings
unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 1024;

glm::mat4 viewMat, projMat, modelmat;

HeightField      hField;
Arcball          arcball;
Shader          *prog = new Shader();

float            z                     = -2;
glm::quat        qCRot                 = glm::quat(glm::mat4(1));
glm::mat4        rotMatrix             = glm::mat4(1);
glm::vec3        camMovement           = glm::vec3(0,0,0);
bool             LEFT                  = false;
bool             RIGHT                 = false;
int              tessLevel             = 1;
glm::ivec2       lastMousePosition;
glm::mat4        objTrans;




//---------------------------------------------------------------------
// setupCamera
//---------------------------------------------------------------------
static void setupCamera()
{
  glm::vec3 lookFrom, lookAt, up;
  z = z + camMovement.z;
  lookFrom = glm::vec3(0, 0, z);
  lookAt = glm::vec3(0, 0, 0);
  up = glm::vec3(0, 1, 0);

  viewMat = glm::lookAt(lookFrom, lookAt, up);
  projMat = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 50.0f);
}


//---------------------------------------------------------------------
// Resize_Callback
//---------------------------------------------------------------------
static void Resize_Callback(GLFWwindow *pW, int w, int h)
{
  float aR = (float)w / (float)h;

  glViewport(0, 0, w, h);
  SCR_WIDTH = w;
  SCR_HEIGHT = h;

  projMat = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 50.0f);
}

//---------------------------------------------------------------------
// init
//---------------------------------------------------------------------
void init(void)
{
  prog->compileShaderFromFile("glsl/shader.vert",   GLSLShader::GLSLShaderType::VERTEX);
  prog->compileShaderFromFile("glsl/shader.frag",   GLSLShader::GLSLShaderType::FRAGMENT);
  prog->compileShaderFromFile("glsl/shader.tessC",  GLSLShader::GLSLShaderType::TESS_CONTROL);
  prog->compileShaderFromFile("glsl/shader.tessE",  GLSLShader::GLSLShaderType::TESS_EVAL);
      
  prog->link();
  prog->use();
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  hField.create("Resources/HeightFields/heightfield.raw",1024,1024);
  hField.loadTexture("Resources/Textures/dirt.jpg");
}

//---------------------------------------------------------------------
// MouseButton_Callback
//---------------------------------------------------------------------
static void MouseButton_Callback(GLFWwindow *pW, int button, int action, int mods)
{
  glm::dvec2 vP;

  glfwGetCursorPos(pW, &vP.x, &vP.y);
  if (button == GLFW_MOUSE_BUTTON_1)
  {
    if (action == GLFW_PRESS)
    {
      arcball = Arcball(glm::vec2(SCR_WIDTH, SCR_HEIGHT), (vP.x - SCR_WIDTH / 2), (vP.y - SCR_HEIGHT / 2), true);
      LEFT = true;
    }
    else
      LEFT = false;
  }
  if (button == GLFW_MOUSE_BUTTON_2)
  {
    if (action == GLFW_PRESS)
      RIGHT = true;
    else
      RIGHT = false;
  }
}


//---------------------------------------------------------------------
// MouseMotion_Callback
//---------------------------------------------------------------------
static void MouseMotion_Callback(GLFWwindow *pW, double x, double y)
{
  glm::dvec2 vP;
  glm::quat rot = glm::quat();

  glfwGetCursorPos(pW, &vP.x, &vP.y);

  if (LEFT)
  {
    rot = arcball.update(glm::vec2(SCR_WIDTH, SCR_HEIGHT), (vP.x - SCR_WIDTH / 2), (vP.y - SCR_HEIGHT / 2));
    qCRot = rot * qCRot;
    rotMatrix = (glm::toMat4(qCRot));
  }

  lastMousePosition = vP;
}

//---------------------------------------------------------------------
// MouseScroll_Callback
//---------------------------------------------------------------------
static void MouseScroll_Callback(GLFWwindow *pW, double x, double y)
{
  camMovement = glm::vec3(0, 0, 0.02 * y);
  setupCamera();
}

//---------------------------------------------------------------------
// Keyboard_Callback
//---------------------------------------------------------------------
static void Keyboard_Callback(GLFWwindow *pW, int key, int scancode, int action, int mods)
{
  if(action==GLFW_RELEASE)
  {
    if(key == GLFW_KEY_UP)
    {
      tessLevel++;

      if(tessLevel == 9)
        tessLevel = 1;
    }
    else if(key == GLFW_KEY_DOWN)
    {
      tessLevel--;
      
      if(tessLevel == 0)
        tessLevel = 8;
    }
    else if(key == GLFW_KEY_S)
    {
    BYTE *pixels = new BYTE[3*SCR_WIDTH*SCR_HEIGHT];
    std::string filename = "ScreenShots/Screenshot_" + std::to_string(std::time(nullptr)) + ".png";
      
      glReadPixels(0,0,SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

      stbi_flip_vertically_on_write(true);
      stbi_write_png(filename.c_str(), SCR_WIDTH, SCR_HEIGHT, 3, pixels, 0);
    }
  }
}

//---------------------------------------------------------------------
// Drop_Callback
//---------------------------------------------------------------------
void Drop_Callback(GLFWwindow *pW, int count, const char** paths)
{
  
  
}

//---------------------------------------------------------------------
// display
//---------------------------------------------------------------------
void display(GLFWwindow *pWindow)
{
  while (!glfwWindowShouldClose(pWindow))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glColor3f(0,1,0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    hField.render(prog, viewMat, projMat, rotMatrix, tessLevel, tessLevel);

    glFlush();

    glfwSwapBuffers(pWindow);
    glfwPollEvents();
  }
}

//---------------------------------------------------------------------
// glInitWindow
//---------------------------------------------------------------------
GLFWwindow* glInitWindow(const int &X, const int &Y, char *name)
{
  GLFWwindow *pW = 0;

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_DEPTH_BITS, 32);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);

  pW = glfwCreateWindow(X, Y, name, 0, 0);

  if (pW)
  {
    glfwMakeContextCurrent(pW);
    glfwSwapInterval(1);
    glfwShowWindow(pW);
  }

  return pW;
}



//---------------------------------------------------------------------
// main
//---------------------------------------------------------------------
int main()
{
  GLFWwindow              *pWindow = 0;

  if (glfwInit())
  {
    pWindow = glInitWindow(SCR_WIDTH, SCR_HEIGHT, "Terrain101");

    if (pWindow)
    {
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
      }

      glfwSetKeyCallback(pWindow, Keyboard_Callback);
      glfwSetMouseButtonCallback(pWindow, MouseButton_Callback);
      glfwSetCursorPosCallback(pWindow, MouseMotion_Callback);
      glfwSetScrollCallback(pWindow, MouseScroll_Callback);
      glfwSetWindowSizeCallback(pWindow, Resize_Callback);
      glfwSetDropCallback(pWindow, Drop_Callback);

      init();
      setupCamera();
      display(pWindow);
      
      glfwDestroyWindow(pWindow);
    }
    glfwTerminate();
  }

  return 0;
}

