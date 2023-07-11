#pragma once
#include "chunkLayer.h"
#include "shader.h"
#include "camera.h"
#include "chunkNibbleMeshData.h"
#include "boxCollider.h"

class chunkNibble
{
public:
	enum class Cube_Faces { Top, Bottom, Left, Right, Front, Back };

	~chunkNibble();
	chunkNibble() { }
	chunkNibble(int chunkX, int nibbleY, int chunkZ);

	void Initialize(int chunkX, int nibbleY, int chunkZ);

	void setNibbleBlock(int x, int y, int z, int data);
	int getNibbleBlock(int x, int y, int z);
	chunkLayer* getNibbleLayers() { return chunkLayers; }

	bool isBlockAir(int x, int y, int z) { return chunkLayers[y].isBlockAir(x, z); }
	bool isAirAndInBounds(int x, int y, int z) { return inBounds(x, y, z) && isBlockAir(x, y, z); }
	bool hasBlocksAtEdge() { return numLayersUsedEdges != 0; }
	bool hasBlocks() { return numLayersUsed != 0; }

	// returns -1 if none found
	int getFirstNonFullLayer()
	{
		for (int i = 0; i < 16; i++)
			if (!chunkLayers[i].isLayerFull())
				return i;
		return -1;
	}

	void Render(Shader* m_shader, Camera* m_camera);

	void rebuildBuffers();
	void setRebuildFlag() { rebuildQueued = true; lastNibbleUpdate = glfwGetTime(); }
	bool isRebuildFlagSet() { return rebuildQueued; }

	bool ShaderInfo(Shader* shader);

	void buildBorder(chunkNibble* other);

	int getChunkX() { return chunkX; }
	int getChunkZ() { return chunkZ; }
	int getNibbleY() { return nibbleY; }

	void buildInsideFaces();

	bool isRenderable() { return true; }

	void InputChunkMeshData(ChunkNibbleMeshData meshData)
	{
		this->verts_raw_d1 = meshData.verts_d1;
		this->verts_raw_d2 = meshData.verts_d2;
		this->indices_raw = meshData.indices;

		this->verts_raw_d1_transparent = meshData.verts_d1_transparent;
		this->verts_raw_d2_transparent = meshData.verts_d2_transparent;
		this->indices_raw_transparent = meshData.indices_transparent;

		this->numVertices = verts_raw_d1.size();
		this->numIndices = indices_raw.size();

		InputChunkColliderData(meshData);

		rebuildBuffers();
	}

private:
	void initializeLayers();
	bool inBounds(int x, int y, int z) { return x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16; }
	bool isBorderBlock(int x, int y, int z) { return x == 15 || x == 0 || y == 15 || y == 0 || z == 15 || z == 0; }

	void setupVertices();
	void setupBuffers();

	void InputChunkColliderData(ChunkNibbleMeshData meshData);

	int chunkX;
	int chunkZ;
	int nibbleY;
	bool renderable = false;
	bool rebuilding = false;

	chunkLayer* chunkLayers = new chunkLayer[16];
	std::vector<BoxCollider> colliders;
	

	int numLayersUsed = 0;
	int numLayersUsedEdges = 0;

	// Graphics/Rendering
	GLuint vao;
	GLuint VB;
	GLuint IB;

	// transparent part of mesh
	GLuint vao_transparent;
	GLuint VB_transparent;
	GLuint IB_transparent;

	bool initialSetup = false;
	bool rebuildQueued = false;
	bool rebuildWaiting = false;

	glm::mat4 model;
	glm::vec3 posVertex;
	glm::mat4 positionMatrix;
	
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;

	std::vector<Vertex> Vertices_transparent;
	std::vector<unsigned int> Indices_transparent;

	int numVertices = 0;
	int numIndices = 0;

	int numVertices_transparent = 0;
	int numIndices_transparent = 0;

	double lastNibbleUpdate = 0;

	std::vector<int> indices_raw;
	std::vector<int> verts_raw_d1;
	std::vector<int> verts_raw_d2;

	std::vector<int> indices_raw_transparent;
	std::vector<int> verts_raw_d1_transparent;
	std::vector<int> verts_raw_d2_transparent;


	GLint m_projectionMatrix;
	GLint m_viewMatrix;
	GLint m_modelMatrix;
};
