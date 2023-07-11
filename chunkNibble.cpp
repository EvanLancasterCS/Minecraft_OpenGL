#include "chunkNibble.h"
#include "block_info.h"
#include "chunkHandler.h"
#include <thread>
#include "profiler_manager.h"
#include "chunkMeshGenerator.h"


chunkNibble::~chunkNibble()
{
    if (VB != 0) {
        glDeleteBuffers(1, &VB);
        glDeleteBuffers(1, &IB);
        glDeleteVertexArrays(1, &vao);

        glDeleteBuffers(1, &VB_transparent);
        glDeleteBuffers(1, &IB_transparent);
        glDeleteVertexArrays(1, &vao_transparent);

        for (int i = 0; i < 16; i++)
            chunkLayers[i].~chunkLayer();
    }
}

chunkNibble::chunkNibble(int _chunkX, int _nibbleY, int _chunkZ)
{
    
}

void chunkNibble::Initialize(int _chunkX, int _nibbleY, int _chunkZ)
{
    initializeLayers();
    chunkX = _chunkX;
    nibbleY = _nibbleY;
    chunkZ = _chunkZ;

    setupBuffers();

    model = glm::mat4(1.f) * glm::translate(glm::vec3(_chunkX * 32, _nibbleY * 32, _chunkZ * 32)) * glm::scale(glm::vec3(2, 2, 2));
    posVertex = glm::vec3(0, 0, 0);
    rebuildQueued = false;
    initialSetup = true;
}

void chunkNibble::rebuildBuffers()
{
    Vertices.clear();
    Indices.clear();
    Vertices_transparent.clear();
    Indices_transparent.clear();

    numIndices = indices_raw.size();
    numVertices = verts_raw_d1.size();
    numIndices_transparent = indices_raw_transparent.size();
    numVertices_transparent = verts_raw_d1_transparent.size();
  

    setupVertices();
    setupBuffers();
}

void chunkNibble::setupVertices() {
    const std::vector<int> ind = indices_raw;
    const int numIndices = indices_raw.size();

    for (int i = 0; i < numIndices; i++) {
        glm::int32 d1 = verts_raw_d1[ind[i]];
        glm::int32 d2 = verts_raw_d2[ind[i]];

        Vertices.emplace_back(d1, d2);
        Indices.push_back(i);
    }

    const std::vector<int> ind_transparent = indices_raw_transparent;
    const int numIndices_transparent = indices_raw_transparent.size();

    for (int i = 0; i < numIndices_transparent; i++) {
        glm::int32 d1 = verts_raw_d1_transparent[ind[i]];
        glm::int32 d2 = verts_raw_d2_transparent[ind[i]];

        Vertices_transparent.emplace_back(d1, d2);
        Indices_transparent.push_back(i);
    }
}

void chunkNibble::setupBuffers() {
    // For OpenGL 3

    // Opaque Mesh
    if (!initialSetup)
        glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    if (!initialSetup)
        glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), (Vertices).data(), GL_STATIC_DRAW);

    if (!initialSetup)
        glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), (Indices).data(), GL_STATIC_DRAW);

    // Transparent Mesh
    if (!initialSetup)
        glGenVertexArrays(1, &vao_transparent);
    glBindVertexArray(vao_transparent);

    if (!initialSetup)
        glGenBuffers(1, &VB_transparent);
    glBindBuffer(GL_ARRAY_BUFFER, VB_transparent);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices_transparent.size(), (Vertices_transparent).data(), GL_STATIC_DRAW);

    if (!initialSetup)
        glGenBuffers(1, &IB_transparent);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB_transparent);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices_transparent.size(), (Indices_transparent).data(), GL_STATIC_DRAW);
}

void chunkNibble::Render(Shader* m_shader, Camera* m_camera)
{
    if (rebuildQueued && !rebuildWaiting && hasBlocks())
    {
        ChunkMeshGenerator::QueueMeshData(chunkX, nibbleY, chunkZ);
        rebuildWaiting = true;
    }
    else if (rebuildQueued && rebuildWaiting)
    {
        if (ChunkMeshGenerator::IsMeshDone(chunkX, nibbleY, chunkZ))
        {
            ChunkNibbleMeshData& meshData = ChunkMeshGenerator::RetrieveMeshData(chunkX, nibbleY, chunkZ);
            this->InputChunkMeshData(meshData);
            rebuildQueued = false;
            rebuildWaiting = false;
            renderable = true;

            // Need to mark for another rebuild if changes have happened inbetween the time the layer data was queried and the last update to the blocks
            if (meshData.layerDataTimestamp < lastNibbleUpdate)
                setRebuildFlag();
            

            ChunkMeshGenerator::RemoveDataFromMap(chunkX, nibbleY, chunkZ);
        }
    }


    if (numLayersUsed != 0 && isRenderable()) {

        // local model matrix
        glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(model));
        
        // OPAQUE geometry
        // vao opaque
        glBindVertexArray(vao);

        // attributes for d1 and d2
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // VBO bind opaque
        glBindBuffer(GL_ARRAY_BUFFER, VB);

        // pass in expected locations
        glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, d1)));
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, d2)));

        // index buffer opaque
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

        // Render opaque
        glDrawElements(GL_TRIANGLES, indices_raw.size(), GL_UNSIGNED_INT, nullptr);

        // TRANSPARENT geometry
        glBindVertexArray(vao_transparent);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VB_transparent);

        glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, d1)));
        glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, d2)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB_transparent);

        glDrawElements(GL_TRIANGLES, indices_raw_transparent.size(), GL_UNSIGNED_INT, nullptr);


        // Disable attributes/buffers
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void chunkNibble::setNibbleBlock(int x, int y, int z, int data)
{
	//if (!inBounds(x, y, z))
	//	printf("ERROR: setting block out of nibble bounds");

	chunkLayer* layer = &chunkLayers[y];

    bool blockChanged = layer->getLayerBlock(x, z) != data;
    if (blockChanged) {
        bool previousEdges = layer->hasBlocksAtEdge();
        
        // If a block was changed at the border, from air-->block or block-->air,
        // AND there is a chunk which would need to update based on this, then notify them.
        if ((data == 0 || layer->getLayerBlock(x, z) == 0)) // air->block or block->air
        {
            // X border
            if (x == 0 || x == 15)
                ChunkHandler::instance->requestChunkUpdate(chunkX + (x == 0 ? -1 : 1), nibbleY, chunkZ);
            
            // Y border
            if (y == 0 || y == 15)
                ChunkHandler::instance->requestChunkUpdate(chunkX, nibbleY + (y == 0 ? -1 : 1), chunkZ);

            // Z border
            if (z == 0 || z == 15)
                ChunkHandler::instance->requestChunkUpdate(chunkX, nibbleY, chunkZ + (z == 0 ? -1 : 1));

        }

        // Set the layer block, track some variables
        bool wasEmpty = layer->isLayerEmpty();
        layer->setLayerBlock(x, z, data);
        bool isEmpty = layer->isLayerEmpty();

        // Set rebuild flag since we changed something
        setRebuildFlag();

        // Update some variables used for optimization
        if (wasEmpty && !isEmpty)
            numLayersUsed += 1;
        else if (!wasEmpty && isEmpty)
            numLayersUsed -= 1;

        bool currentEdges = layer->hasBlocksAtEdge();

        if (!previousEdges && currentEdges)
            numLayersUsedEdges++;
        else if (previousEdges && !currentEdges)
            numLayersUsedEdges--;
    }
}

int chunkNibble::getNibbleBlock(int x, int y, int z)
{
	if (!inBounds(x, y, z))
		printf("ERROR: getting block out of nibble bounds: %i , %i , %i", x, y, z);

	return chunkLayers[y].getLayerBlock(x, z);
}

void chunkNibble::InputChunkColliderData(ChunkNibbleMeshData meshData)
{
    colliders.clear();
    for (auto obj : meshData.colliderData)
    {
        auto key = obj.first;
        int x = key.first.first;
        int y = key.first.second;
        int z = key.second;

        int realX = x + (chunkX * 16);
        int realY = y + (nibbleY * 16);
        int realZ = z + (chunkZ * 16);

        BoxCollider collider(realX, realY, realZ);

        colliders.push_back(collider);
    }

}

void chunkNibble::initializeLayers()
{

}

// get shader var locations
bool chunkNibble::ShaderInfo(Shader* shader)
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

    return anyProblem;
}