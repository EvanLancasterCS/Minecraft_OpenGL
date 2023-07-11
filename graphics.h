#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <stack>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "wireframe_cube.h"

#define numVBOs 2;
#define numIBs 2;

class ChunkHandler;

class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height);
    void Render();

    Camera* getCamera() { return m_camera; }

  private:
    std::string ErrorString(GLenum error);

    bool collectShPrLocs(); // Sets shader variable locations

    Camera *m_camera;
    Shader *m_shader;
    Shader* m_wireframe_shader;
    ChunkHandler* m_chunks;


    // Shader Variables
    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;

    GLint m_wireframe_projectionMatrix;
    GLint m_wireframe_viewMatrix;
    GLint m_wireframe_modelMatrix;

    GLint m_positionAttrib;
    GLint m_tcAttrib;
    GLint m_hasTexture;
};

#endif /* GRAPHICS_H */
