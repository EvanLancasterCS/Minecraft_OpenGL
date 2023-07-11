#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include "chunk.h"
#include <future>
#include <thread>
#include <chrono>
#include "graphics.h"

class ChunkHandler
{
public:
	static ChunkHandler* instance;

	ChunkHandler(Shader& m_shader);

	void Render(Shader* m_shader, Camera* m_camera);

	GLuint getTextureID() { return m_texture->getTextureID(); }

	// Pushes appropriate faces to a chunk nibble for bordering nibbles.
	// Should be used on full rebuild / fresh creation
	void buildChunkNibbleBorders(chunkNibble* nibble);

	std::vector<chunkNibble*> getChunkNibbleBorders(int chunkX, int nibbleY, int chunkZ);

	bool isBlockOpaque(int x, int y, int z);
	bool isBlockAt(int x, int y, int z);
	int getBlockAt(int x, int y, int z);

	void setBlock(int x, int y, int z, int data)
	{
		int chunkX = glm::floor(x / 16.f);
		int chunkZ = glm::floor(z / 16.f);
		Chunk* c = getChunk(chunkX, chunkZ);
		if (c != nullptr)
		{

			x %= 16;
			z %= 16;
			if (x < 0) x += 16;
			if (z < 0) z += 16;

			c->setBlock(x, y, z, data);
		}
	}

	void requestChunkUpdate(int chunkX, int nibbleY, int chunkZ)
	{
		Chunk* c = getChunk(chunkX, chunkZ);
		if (c != nullptr)
		{
			chunkNibble* nibble = c->getNibble(nibbleY);
			c->requestRebuild(nibbleY);
		}
	}

	void camera_GenerateChunk(int x, int z)
	{
		if (!chunkInCreationQueue(x, z) && !chunkExists(x, z))
		{
			chunkCreationQueue.insert(chunkCreationQueue.begin(), glm::vec2(x, z));
		}
	}

	void sortRebuildsByDistance(int playerX, int playerY, int playerZ);

	void unloadChunksByDistance(int playerX, int playerY, int playerZ);

	std::vector<Chunk*> getChunksByDistance(int playerX, int playerY, int playerZ);

	void updateCameraFrustrum(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
	{
		cameraPoints[0] = p1;
		cameraPoints[1] = p2;
		cameraPoints[2] = p3;
	}

	Chunk* getChunk(int x, int z)
	{
		if (!chunkExists(x, z))
			return nullptr;
		return chunkMap.at(std::pair<int, int>(x, z));
	}

	glm::vec3 getPlayerPos() { return playerPos; }

	bool isPlayerInChunk(int cx, int cy, int cz)
	{
		int playerCX = glm::floor(playerPos.x / 16.0f);
		int playerCY = glm::floor(playerPos.y / 16.0f);
		int playerCZ = glm::floor(playerPos.z / 16.0f);
		
		return playerCX == cx && playerCY == cy && playerCZ == cz;
	}

private:
	Shader* m_shader;

	struct pair_hash
	{
		template <class T1, class T2>
		std::size_t operator() (const std::pair<T1, T2>& p) const
		{
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);
			return h1 ^ h2;
		}
	};
	using intPair = std::pair<int, int>;

	std::unordered_map<intPair, Chunk*, pair_hash> chunkMap;

	// creates the chunk object for x, z and adds it to our vector of chunks
	void generateChunk(int x, int z);

	bool chunkExists(int x, int z)
	{
		return chunkMap.find(std::pair<int, int>(x,z)) != chunkMap.end();
	}

	bool chunkInCreationQueue(int x, int z)
	{
		for (int i = 0; i < chunkCreationQueue.size(); i++)
			if (chunkCreationQueue[i].x == x && chunkCreationQueue[i].y == z)
				return true;
		return false;
	}

	// Initialization of chunks, blocks setup, VBO/etc.
	// Returns num of creations
	int pushChunkCreations(int max_chunk_creations);

	// Timer to track last time we rebuilt/created something
	float lastChunkInteraction = 0.f;
	float TIME_BETWEEN_CREATIONS = 0.1f;

	int chunkUnloadDistance = 16;


	
	std::vector<glm::vec2> chunkCreationQueue;

	Texture* m_texture;

	bool hasTexture;
	bool hasNormal;

	glm::vec3 playerPos = glm::vec3(0, 0, 0);
	std::vector<glm::vec2> cameraPoints = std::vector<glm::vec2>(3);

	bool test = false;
};