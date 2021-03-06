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

#include <anttweakbar.h>



// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 1280;

glm::mat4 viewMat, projMat, modelmat;



HeightField      hField;
Arcball          arcball;
Shader          *basicLOD = new Shader();
Shader          *compGPULOD = new Shader();

float            z                     = -2;
glm::quat        qCRot                 = glm::quat(glm::mat4(1));
glm::mat4        rotMatrix             = glm::mat4(1);
glm::vec3        camMovement           = glm::vec3(0,0,0);
bool             LEFT                  = false;
bool             RIGHT                 = false;
int              tessLevel             = 1;
glm::ivec2       lastMousePosition;
glm::mat4        objTrans;

int              innerTess              = 1;
int              outerTess              = 1;
int              innerHolder            = 1;
bool             innerSwitch            = false;
int              outerHolder            = 1;
bool             outerSwitch            = false;

short            rotation               = 0;
glm::mat3        mtex                   = glm::mat3(1);


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
  projMat = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 500.0f);
}


//---------------------------------------------------------------------
// Resize_Callback
//---------------------------------------------------------------------
static void Resize_Callback(GLFWwindow *pW, int w, int h)
{
TwWindowSize(w, h);


  float aR = (float)w / (float)h;

  glViewport(0, 0, w, h);
  SCR_WIDTH = w;
  SCR_HEIGHT = h;

  projMat = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 6000.0f);
}

//---------------------------------------------------------------------
// initCompGPU
//---------------------------------------------------------------------
bool initCompGPU()
{
  compGPULOD->compileShaderFromFile("glsl/DynamicLOD.ver",   GLSLShader::GLSLShaderType::VERTEX);
  compGPULOD->compileShaderFromFile("glsl/DynamicLOD.fra",   GLSLShader::GLSLShaderType::FRAGMENT);
  compGPULOD->compileShaderFromFile("glsl/DynamicLOD.tcs",  GLSLShader::GLSLShaderType::TESS_CONTROL);
  compGPULOD->compileShaderFromFile("glsl/DynamicLOD.tes",  GLSLShader::GLSLShaderType::TESS_EVAL);

  compGPULOD->link();
  compGPULOD->use();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);


  if(!hField.createCompGPU("Resources/HeightFields/KonarValleyDem.tif"))
      return false;

  hField.loadTexture("Resources/Textures/kl.png");

  return true;
}

//---------------------------------------------------------------------
// texCoordRotateMatrix
//--------------------------------------------------------------------- 
inline void texCoordRotateMatrix(short _tcRot, glm::mat3 &mtex)
{
  float angle = 90.0f;
  mtex = glm::mat3(glm::rotate((_tcRot * angle),glm::vec3(0,1,0)));
}

//---------------------------------------------------------------------
// MouseButton_Callback
//---------------------------------------------------------------------
static void MouseButton_Callback(GLFWwindow *pW, int button, int action, int mods)
{
TwEventMouseButtonGLFW3(pW, button, action, mods);

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
TwEventMousePosGLFW3(pW, x, y);


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
TwEventMouseWheelGLFW3(pW, x, y);

  camMovement = glm::vec3(0, 0, 0.02 * y);
  setupCamera();
}

//---------------------------------------------------------------------
// switchUpdates 
//---------------------------------------------------------------------
void switchUpdates()
{
  if(innerHolder == innerTess)
  {
    innerSwitch = false;
  }
  else
  {
    innerHolder = innerTess;
    innerSwitch = true;
  }

  if(outerHolder == outerTess)
  {
    innerSwitch = false;
  }
  else
  {
    outerHolder = outerTess;
    outerSwitch = true;
  }
}


//---------------------------------------------------------------------
// Keyboard_Callback
//---------------------------------------------------------------------
static void Keyboard_Callback(GLFWwindow *pW, int key, int scancode, int action, int mods)
{
TwEventKeyGLFW3(pW, key, scancode, action, mods);

  if(action==GLFW_RELEASE)
  {
    if(key == GLFW_KEY_RIGHT)
    {
      rotation++;
      if(rotation == 5) rotation = 0;
    }
    else if(key == GLFW_KEY_LEFT)
    {
      --rotation;
      if(rotation == -1) rotation = 4;
    }
    else if(key == GLFW_KEY_S)
    {
    BYTE *pixels = new BYTE[3*SCR_WIDTH*SCR_HEIGHT];
    std::string filename = "ScreenShots/Screenshot_" + std::to_string(std::time(nullptr)) + ".png";
      
      glReadPixels(0,0,SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

      stbi_flip_vertically_on_write(true);
      stbi_write_png(filename.c_str(), SCR_WIDTH, SCR_HEIGHT, 3, pixels, 0);
    }
    else if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(pW,GL_TRUE);    
    else if (key == GLFW_KEY_W)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else if (key == GLFW_KEY_F)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

    switchUpdates();

    texCoordRotateMatrix(rotation, mtex);
    hField.render(compGPULOD, viewMat, projMat, rotMatrix, mtex);

    TwDraw();
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
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

      TwInit(TW_OPENGL, NULL);
      TwWindowSize(SCR_WIDTH, SCR_HEIGHT);

      TwBar *myBar;
      myBar = TwNewBar("Controls");

      TwEnumVal InnerEV[] = 
      {
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {4, "4"},
        {5, "5"},
        {6, "6"},
        {7, "7"},
        {8, "8"}
      };

      TwType innerVal = TwDefineEnum("Mode", InnerEV, 8);

      TwAddVarRW(myBar, "Inner Tess", innerVal, &innerTess, "help = 'Change Tess' group = 'Tessellation'");

      TwEnumVal OuterEV[] = 
      {
        {1, "1"},
        {2, "2"},
        {3, "3"},
        {4, "4"},
        {5, "5"},
        {6, "6"},
        {7, "7"},
        {8, "8"}
      };

      TwType outerVal = TwDefineEnum("Mode", OuterEV, 8);

      TwAddVarRW(myBar, "Outer Tess", outerVal, &outerTess, "help = 'Change Tess' group = 'Tessellation'");


      glfwSetKeyCallback(pWindow, Keyboard_Callback);
      glfwSetMouseButtonCallback(pWindow, MouseButton_Callback);
      glfwSetCursorPosCallback(pWindow, MouseMotion_Callback);
      glfwSetScrollCallback(pWindow, MouseScroll_Callback);
      glfwSetWindowSizeCallback(pWindow, Resize_Callback);
      glfwSetDropCallback(pWindow, Drop_Callback);

      if(initCompGPU())
      {
        setupCamera();
        display(pWindow);
      }
      
      glfwDestroyWindow(pWindow);
    }
    glfwTerminate();
  }

  return 0;
}

