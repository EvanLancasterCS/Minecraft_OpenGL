#ifndef ENGINE_H
#define ENGINE_H


#include <assert.h>
#include "window.h"
#include "graphics.h"

static Camera* m_camera;
static bool firstMove = true;
static float lastX = 800 / 2.f;
static float lastY = 600 / 2.f;
static void cursorPositionCallBack(GLFWwindow*, double xpos, double ypos);
static void scrollwheel_callback(GLFWwindow* window,double xoffset,  double yoffset);

class Engine
{
  public:
    Engine(const char*  name, int width, int height);
    
    ~Engine();
    bool Initialize();
    void Run();
    void ProcessInput();
    unsigned int getDT();
    long long GetCurrentTimeMillis();
    void Display(GLFWwindow*, double);
    void cursor_position_callback(GLFWwindow*, double xpos, double ypos);
    
  
  private:
    // Window related variables
    Window *m_window;    
    const char* m_WINDOW_NAME;
    int m_WINDOW_WIDTH;
    int m_WINDOW_HEIGHT;
    bool m_FULLSCREEN;
    float deltaTime = 0.f;
    float lastFrame = 0.f;
    float lastToggleTime = 0.f;
    int frameCount = 0;
    int lastTime = 0;

    bool lastLeftClick = false;
    bool lastRightClick = false;

    Graphics *m_graphics;

    bool m_running;
};

#endif // ENGINE_H
