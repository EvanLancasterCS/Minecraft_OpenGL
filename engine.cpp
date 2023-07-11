

#include "engine.h"
#include "glm/ext.hpp"

Engine::Engine(const char* name, int width, int height)
{
  m_WINDOW_NAME = name;
  m_WINDOW_WIDTH = width;
  m_WINDOW_HEIGHT = height;

}


Engine::~Engine()
{
  delete m_window;
  delete m_graphics;
  m_window = NULL;
  m_graphics = NULL;
}

bool Engine::Initialize()
{
  // Start a window
  m_window = new Window(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT);
  if(!m_window->Initialize())
  {
    printf("The window failed to initialize.\n");
    return false;
  }

  // Start the graphics
  m_graphics = new Graphics();
  if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT))
  {
    printf("The graphics failed to initialize.\n");
    return false;
  }

  m_camera = m_graphics->getCamera();

  glfwSetCursorPosCallback(m_window->getWindow(), cursorPositionCallBack);
  glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(m_window->getWindow(), scrollwheel_callback);

  // No errors
  return true;
}

void Engine::Run()
{
  m_running = true;

  while (!glfwWindowShouldClose(m_window->getWindow()))
  {
      ProcessInput();
      Display(m_window->getWindow(), glfwGetTime());
      glfwPollEvents();

      if ((int)glfwGetTime() != lastTime)
      {
          std::cout << "FPS: " << frameCount << "\n";
          lastTime = (int)glfwGetTime();
          frameCount = 0;
      }

      frameCount++;
  }
  m_running = false;

}

void Engine::ProcessInput()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window->getWindow(), true);


    // Camera Inputs
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        m_camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        m_camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        m_camera->ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        m_camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        m_camera->ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(m_window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        m_camera->ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetMouseButton(m_window->getWindow(), 0) == GLFW_PRESS)
    {
        if(!lastLeftClick)
            m_camera->ProcessKeyboard(LEFTCLICK, deltaTime);
        lastLeftClick = true;
    }
    else
    {
        lastLeftClick = false;
    }

    if (glfwGetMouseButton(m_window->getWindow(), 1) == GLFW_PRESS)
    {
        if(!lastRightClick)
            m_camera->ProcessKeyboard(RIGHTCLICK, deltaTime);
        lastRightClick = true;
    }
    else
    {
        lastRightClick = false;
    }
    
}

void Engine::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
 
    glfwGetCursorPos(window, &xpos, &ypos);
    std::cout << "Position: (" << xpos << ":" << ypos << ")";
}



unsigned int Engine::getDT()
{
    return glfwGetTime();
}

long long Engine::GetCurrentTimeMillis()
{
    return (long long) glfwGetTime();
}

void Engine::Display(GLFWwindow* window, double time) {

    m_graphics->Render();
    m_window->Swap();

}

// Function which computes the dx/dy of the mouse from the last frame and can be used to callback
// for input
static void cursorPositionCallBack(GLFWwindow* window, double xpos, double ypos) {
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);

    if (firstMove)
    {
        lastX = x;
        lastY = y;
        firstMove = false;
    }

    float xOffset = x - lastX;
    float yOffset = lastY - y;

    lastX = x;
    lastY = y;

    // Mouse x/y per frame
    m_camera->ProcessMouseMovement(xOffset, yOffset);
}

// Function which computes the dx/dy of the mouse wheel from last frame and can be used to callback
// for input
static void scrollwheel_callback(GLFWwindow* window, double xoffset, double yoffset) {
    m_camera->ProcessScrollMovement(yoffset);
}
