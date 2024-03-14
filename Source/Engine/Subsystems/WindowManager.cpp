#include "WindowManager.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include <GLFW/glfw3.h>

void WindowManager::Initialize()
{
  int success = glfwInit();
  assert(success && "GLFW initialization failed");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  /* Load OpenGL 4.6 */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);  /* Enable 4x MSAA on GLFW frame buffer */

  /* Set default window values */
  _context = glfwCreateWindow(1600, 900, "GameEngine", nullptr, nullptr); /* 1600x900 */
  assert(_context && "GLFW creation window failed");

  glfwMakeContextCurrent(_context);
  glfwSetWindowPos(_context, 50, 50);   
  glfwSetWindowAspectRatio(_context, 16, 9);   /* 16:9 */
  glfwSwapInterval(0);  /* No vsync */

  glfwSetWindowSizeCallback(_context, [](GLFWwindow* window, int width, int height) {
    glfwSetWindowSize(window, width, height);
  });

  /* Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error. */
  int version = gladLoadGL(glfwGetProcAddress);
  assert((version != 0) && "Failed to initialize OpenGL context");

  /* Depth test ON */
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);   /* enable or disable writing into the depth buffer */
  glDepthFunc(GL_LESS);   /* specify the value used for depth buffer comparisons */

  /* Stencil test ON */
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_EQUAL, 0, 0xFF); /* glStencilFunc only describes whether OpenGL should pass or discard 
                                       fragments based on the stencil buffer's content, not how we can actually 
                                       update the buffer */
  
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); /* whatever the outcome of any of the tests, 
                                             the stencil buffer keeps its values. The default behavior does 
                                             not update the stencil buffer, so if you want to write to the 
                                             stencil buffer you need to specify at least one different action 
                                             for any of the options */

  //glStencilMask(0xFF);  /* enable write to stencil buffer */
  //glStencilMask(0x00);  /* disable write to stencil buffer */

  /* Blending OFF */
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Culling OFF */
  glDisable(GL_CULL_FACE);
  glCullFace(GL_FRONT); /* specify whether front- or back-facing facets can be culled */
  glFrontFace(GL_CW);

  /* Gamma correction OFF */
  glDisable(GL_FRAMEBUFFER_SRGB);

  /* Antialising ON */
  glEnable(GL_MULTISAMPLE);
}

void WindowManager::CleanUp()
{
  glfwDestroyWindow(_context);
  glfwTerminate();
}

void WindowManager::PoolEvents() const
{
  glfwPollEvents();
}

void WindowManager::Close() const
{ 
  glfwSetWindowShouldClose(_context, GLFW_TRUE);
}

bool WindowManager::IsOpen() const 
{ 
  return !glfwWindowShouldClose(_context); 
}

void WindowManager::SetWindowTitle(const char* title) const
{
  glfwSetWindowTitle(_context, title);
}

void WindowManager::SetWindowAspectRatio(int numer, int denom) const
{
  glfwSetWindowAspectRatio(_context, numer, denom);
}

void WindowManager::SwapWindowBuffers() const 
{ 
  glfwSwapBuffers(_context); 
}

Vec2i WindowManager::GetWindowSize() const
{
  Vec2i res{};
  glfwGetWindowSize(_context, &res.x, &res.y);
  return res;
}

void WindowManager::SetWindowSize(int w, int h) const
{
  glfwSetWindowSize(_context, w, h);
}

Vec2i WindowManager::GetFramebufferSize() const
{
  Vec2i res{};
  glfwGetFramebufferSize(_context, &res.x, &res.y);
  return res;
}

int WindowManager::GetKey(uint32_t key) const 
{ 
  return glfwGetKey(_context, key); 
}

int WindowManager::GetMouseKey(uint32_t key) const 
{ 
  return glfwGetMouseButton(_context, key); 
}

Vec2d WindowManager::GetCursorPosition() const
{
  Vec2d res{};
  glfwGetCursorPos(_context, &res.x, &res.y);
  return res;
}

void WindowManager::SetCursorMode(int value) const 
{ 
  glfwSetInputMode(_context, GLFW_CURSOR, value); 
}

void WindowManager::SetWindowPosition(int x, int y) const
{ 
  glfwSetWindowPos(_context, x, y); 
}

void WindowManager::SetWindowVsync(bool b) const
{
  glfwSwapInterval((int)b);
}
