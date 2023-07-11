#include "chunk.h"
#include "block_info.h"
#include "profiler_manager.h"
#include <functional>

// if something messes up it probably because of this line
#include "chunkHandler.h"

Chunk::Chunk()
{
    init();
    
}

Chunk::Chunk(Chunk* otherChunk)
{
    
}

Chunk::Chunk(int _chunkX, int _chunkZ, const char* fname, const char* nname) {

    chunkX = _chunkX;
    chunkZ = _chunkZ;

    for (int i = 0; i < NUM_NIBBLES; i++)
    {
        nibbles.emplace_back(chunkX, i, chunkZ);
    }
    for (int i = 0; i < NUM_NIBBLES; i++)
    {
        nibbles[i].Initialize(chunkX, i, chunkZ);
    }
    init();
}

void Chunk::rebuildMesh()
{
    
}

void Chunk::rebuildBuffers()
{
    
}
void Chunk::Render(Shader* m_shader, Camera* m_camera)
{
    for (int i = 0; i < NUM_NIBBLES; i++)
        if(nibbles[i].isRenderable())
            nibbles[i].Render(m_shader, m_camera);
}

// vertices and tris
void Chunk::init()
{
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 90; y++)
        {
            for (int z = 0; z < 16; z++)
            {
                int rX = x + (16 * chunkX);
                int rZ = z + (16 * chunkZ);

                int evaluation = 70 + 10 * sin(0.1f * rX) + 10 * cos(0.1f * rZ);

                bool a = y == (evaluation);
                bool b = y < evaluation;
                bool c = y < 5;
                int finalBlock = 0;
                if (c)
                {
                    finalBlock = 3;
                }
                else if (a)
                {
                    finalBlock = 2;
                }
                else if (b)
                {
                    finalBlock = 1;
                }
                else if (y > evaluation && y < 60)
                {
                    finalBlock = 4;
                }

                nibbles[y / 16].setNibbleBlock(x, y % 16, z, finalBlock); // wavey siney waves
            }
        }
    }
    // Update variables for highest Y's since we updated blocks
    updateHighestFlag = true;
    checkHighestFlag();
}

// get shader var locations
bool Chunk::ShaderInfo(Shader* shader)
{
    bool anyProblem = true;
    // Locate the projection matrix in the shader
    m_projectionMatrix = shader->GetUniformLocation("projectionMatrix");
    if (m_projectionMatrix == INVALID_UNIFORM_LOCATION)
    {
        printf("m_projectionMatrix not found\n");
        anyProblem = false;
    }

    // Locate the view matrix in the shader
    m_viewMatrix = shader->GetUniformLocation("viewMatrix");
    if (m_viewMatrix == INVALID_UNIFORM_LOCATION)
    {
        printf("m_viewMatrix not found\n");
        anyProblem = false;
    }

    // Locate the model matrix in the shader
    m_modelMatrix = shader->GetUniformLocation("modelMatrix");
    if (m_modelMatrix == INVALID_UNIFORM_LOCATION)
    {
        printf("m_modelMatrix not found\n");
        anyProblem = false;
    }

    for (int i = 0; i < NUM_NIBBLES; i++)
        nibbles[i].ShaderInfo(shader);

    return anyProblem;
}