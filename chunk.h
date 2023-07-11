#pragma once
#include <vector>
#include "Texture.h"
#include "shader.h"
#include "camera.h"
#include "chunkNibble.h"

const int NUM_NIBBLES = 16;
const int NIBBLE_SIZE = 16;

class Chunk
{
public:
    Chunk();
    Chunk(Chunk* otherChunk);
    Chunk(int _chunkX, int _chunkZ, const char* fname, const char* nname);

    bool ShaderInfo(Shader* shader);
    void Render(Shader* m_shader, Camera* m_camera);

    int getChunkX() const { return chunkX; }
    int getChunkZ() const { return chunkZ; }

    bool blockExists(int x, int y, int z) 
    {  
        checkHighestFlag();

        int xNibble = (int)x % 16;
        int yNibble = (int)y % 16;
        int zNibble = (int)z % 16;
        int nibbleIndex = (int)y / 16;
        chunkNibble* faceNibble = &nibbles[nibbleIndex];
        return faceNibble->getNibbleBlock(xNibble, yNibble, zNibble) != 0;
    }

    int getBlockID(int x, int y, int z)
    {
        int xNibble = (int)x % 16;
        int yNibble = (int)y % 16;
        int zNibble = (int)z % 16;
        int nibbleIndex = (int)y / 16;
        chunkNibble* faceNibble = &nibbles[nibbleIndex];
        return faceNibble->getNibbleBlock(xNibble, yNibble, zNibble);
    }

    void setBlock(int x, int y, int z, int data)
    {
        int xNibble = (int)x % 16;
        int yNibble = (int)y % 16;
        int zNibble = (int)z % 16;
        int nibbleIndex = (int)y / 16;
        chunkNibble* blockNibble = &nibbles[nibbleIndex];
        blockNibble->setNibbleBlock(xNibble, yNibble, zNibble, data);

        rebuildQueued = true;
        updateHighestFlag = true;
    }

    chunkNibble* getNibble(int nibbleY) { return &nibbles[nibbleY]; }

    int getHighestYBorder()
    {
        return highestBorderY;
    }

    int getHighestY()
    {
        return highestY;
    }

    void rebuildBuffers();
    void rebuildMesh();
    void requestRebuild(int nibbleY) { getNibble(nibbleY)->setRebuildFlag(); }
   
private:
    void init();


    static float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

    void checkHighestFlag()
    {
        if (updateHighestFlag)
        {
            updateHighestY();
            updateHighestFlag = false;
        }
    }

    void updateHighestY()
    {
        int highestBorder = 0;
        int highestBlocks = 0;
        for (int i = 0; i < NUM_NIBBLES; i++)
        {
            if (nibbles[i].hasBlocksAtEdge()) {
                highestBorder = (i * NIBBLE_SIZE) + 15;
                highestBlocks = (i * NIBBLE_SIZE) + 15;
            }
            else if (nibbles[i].hasBlocks())
            {
                highestBlocks = (i * NIBBLE_SIZE) + 15;
            }
        }
        highestY = highestBlocks;
        highestBorderY = highestBorder;
    }

    bool initialSetup = false;

    // Chunk variables
    std::vector<chunkNibble> nibbles;// = new chunkNibble[NUM_NIBBLES];

    int chunkX = 0;
    int chunkZ = 0;
    int highestBorderY = 0;
    int highestY = 0;

    // Rebuild flag
    bool rebuildQueued = false;
    // highest Y variable update on render
    bool updateHighestFlag = false;

    double lastFrame = 0;

    // Shader Variables
    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;
};
