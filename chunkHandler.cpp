#include "chunkHandler.h"
#include "chunkMeshGenerator.h"
#include "ray.h"

ChunkHandler* ChunkHandler::instance = nullptr;

ChunkHandler::ChunkHandler(Shader& shader)
{
	if (instance == nullptr)
	{
		instance = this;
	}
	else
	{
		std::cout << "error: two chunk handlers\n";
	}

	//chunkMap = std::unordered_map<pair<int,int>, Chunk*>();
	m_shader = &shader;

	// load texture from file
	m_texture = new Texture("assets\\texture_maps.png", true);
	
	int dist = 1;

	for (int i = -dist; i <= dist; i++)
		for (int j = -dist; j <= dist; j++)
			generateChunk(i, j);

	//generateChunk(0, 0);
}

// Obtained from https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
float sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

// Obtained from https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
bool pointInTriangle(glm::vec2 pt, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign(pt, v1, v2);
	d2 = sign(pt, v2, v3);
	d3 = sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

void ChunkHandler::Render(Shader* m_shader, Camera* m_camera)
{
	// Start the correct program
	m_shader->Enable();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getTextureID());

	GLuint sampler = m_shader->GetUniformLocation("textureSP");

	glUniform1i(sampler, 0);

	if (glfwGetTime() <= 5)
	{
		pushChunkCreations(25);
	}


	if (glfwGetTime() - lastChunkInteraction >= TIME_BETWEEN_CREATIONS) {
		int num_creations = 0;
		num_creations = pushChunkCreations(1);

		if (num_creations != 0)
			lastChunkInteraction = glfwGetTime();
	}
	
	static bool k = false;
	if (!k)
	{
		k = true;
		Debug_Tools::CreateWireframe(10, 10, 10, 1, 1, 1);
		Debug_Tools::CreateWireframe(-10, 10, 10, 1, 1, 1);
		Debug_Tools::CreateWireframe(10, 10, -10, 1, 1, 1);
		Debug_Tools::CreateWireframe(-10, 10, -10, 1, 1, 1);
	}


	ChunkMeshGenerator::MeshUpdateTick(playerPos.x, playerPos.y, playerPos.z);

	std::vector<Chunk*> chunksOrdered = getChunksByDistance(playerPos.x, playerPos.y, playerPos.z);

	for (auto chunk : chunksOrdered)
	{
		chunk->Render(m_shader, m_camera);
	}
}

int ChunkHandler::pushChunkCreations(int max_chunk_creations)
{
	// Create chunks given by the coordinates of the queue, at a rate of 
	// max_chunk_creations per frame
	int creations_used = 0;

	for (int i = chunkCreationQueue.size() - 1; i >= 0; i--)
	{
		// Skip over and erase chunks that are too far away
		int distSquared = glm::pow((playerPos.x / 16) - chunkCreationQueue[i].x, 2) + glm::pow((playerPos.z / 16) - chunkCreationQueue[i].y, 2);
		if (distSquared > glm::pow(chunkUnloadDistance, 2)) {
			chunkCreationQueue.erase(chunkCreationQueue.begin() + i);
			continue;
		}

		// Move on if we have used our allowed amount of creations
		if (max_chunk_creations == creations_used)
			break;

		glm::vec2 chunkPos = chunkCreationQueue[chunkCreationQueue.size() - 1];
		chunkCreationQueue.pop_back();

		generateChunk(chunkPos.x, chunkPos.y);

		creations_used++;
	}

	return creations_used;
}

// bubble sort lol
// Sorts the creation/rebuild queues based on distance to the player, closer = do first.
void ChunkHandler::sortRebuildsByDistance(int playerX, int playerY, int playerZ)
{
	playerPos = glm::vec3(playerX, playerY, playerZ);

	int pChunkX = playerX / 16;
	int pChunkZ = playerZ / 16;

	int size = chunkCreationQueue.size();

	for (int i = size - 1; i >= 0; i--)
	{
		int distI = glm::pow(pChunkX - chunkCreationQueue[i].x, 2) + glm::pow(pChunkZ - chunkCreationQueue[i].y, 2);

		// Skip over and erase chunks that are too far away
		if (distI > glm::pow(chunkUnloadDistance, 2)) {
			chunkCreationQueue.erase(chunkCreationQueue.begin() + i);
			continue;
		}

		for (int j = i - 1; j >= 0; j--)
		{
			int distJ = glm::pow(pChunkX - chunkCreationQueue[j].x, 2) + glm::pow(pChunkZ - chunkCreationQueue[j].y, 2);
			
			// swap
			if (distI > distJ)
			{
				glm::vec2 placeholder = chunkCreationQueue[i];
				chunkCreationQueue[i] = chunkCreationQueue[j];
				chunkCreationQueue[j] = placeholder;
			}
		}
	}
}

std::vector<Chunk*> ChunkHandler::getChunksByDistance(int playerX, int playerY, int playerZ)
{
	playerPos = glm::vec3(playerX, playerY, playerZ);

	int pChunkX = glm::floor(playerX / 16.0f);
	int pChunkZ = glm::floor(playerZ / 16.0f);

	std::vector<Chunk*> sortedList;

	for (auto key : chunkMap)
		sortedList.push_back(key.second);
	
	sort(sortedList.begin(), sortedList.end(), [pChunkX, pChunkZ](const Chunk* lhs, const Chunk* rhs) 
		{
			int distSquaredLHS = glm::pow(pChunkX - lhs->getChunkX(), 2) + glm::pow(pChunkZ - lhs->getChunkZ(), 2);
			int distSquaredRHS = glm::pow(pChunkX - rhs->getChunkX(), 2) + glm::pow(pChunkZ - rhs->getChunkZ(), 2);


			return distSquaredLHS > distSquaredRHS;
		});

	return sortedList;
}

// Unloads chunks that are out of chunk render distance
void ChunkHandler::unloadChunksByDistance(int playerX, int playerY, int playerZ)
{
	int pChunkX = playerX / 16;
	int pChunkZ = playerZ / 16;

	std::vector<intPair> markedChunks;

	for (auto chunk : chunkMap)
	{
		Chunk* c = chunk.second;
		int distSquared = glm::pow(pChunkX - c->getChunkX(), 2) + glm::pow(pChunkZ - c->getChunkZ(), 2);

		if (distSquared > glm::pow(chunkUnloadDistance, 2))
		{
			markedChunks.push_back(chunk.first);
		}
	}

	for (auto chunkCoord : markedChunks)
	{
		Chunk* c = chunkMap.at(chunkCoord);
		chunkMap.erase(chunkCoord);
		delete c;
	}
}

bool ChunkHandler::isBlockAt(int x, int y, int z)
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

		return c->blockExists(x, y, z);
	}
	return false;
}

bool ChunkHandler::isBlockOpaque(int x, int y, int z)
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

		return BLOCK_OPAQUE[c->getBlockID(x, y, z)];
	}
	return false;
}

int ChunkHandler::getBlockAt(int x, int y, int z)
{
	if (y < 0) return -1;

	int chunkX = glm::floor(x / 16.f);
	int chunkZ = glm::floor(z / 16.f);
	Chunk* c = getChunk(chunkX, chunkZ);
	if (c != nullptr)
	{

		x %= 16;
		z %= 16;
		if (x < 0) x += 16;
		if (z < 0) z += 16;

		return c->getBlockID(x, y, z);
	}
	return -1;
}

void ChunkHandler::generateChunk(int x, int z)
{
	if (chunkExists(x, z))
	{
		std::cout << "Error: generating chunk that already exists\n";
		return;
	}

	Chunk* newChunk = new Chunk(x, z, "assets\\dirt-diffuse.jpg", "assets\\dirt-normals.png");
	newChunk->ShaderInfo(m_shader);
	chunkMap.insert({ {x,z}, newChunk });
}

// Returns vector of nibbles which share a border with the given nibble
std::vector<chunkNibble*> ChunkHandler::getChunkNibbleBorders(int chunkX, int nibbleY, int chunkZ)
{
	std::vector<chunkNibble*> nibbles;
	Chunk* helperChunk = nullptr;

	// Top, Bottom nibbles
	helperChunk = getChunk(chunkX, chunkZ);
	if (helperChunk != nullptr)
	{
		if (nibbleY != 0) nibbles.push_back(helperChunk->getNibble(nibbleY - 1));
		if (nibbleY != 15) nibbles.push_back(helperChunk->getNibble(nibbleY + 1));
	}

	// +x nibble
	helperChunk = getChunk(chunkX + 1, chunkZ);
	if (helperChunk != nullptr) nibbles.push_back(helperChunk->getNibble(nibbleY));

	// -x nibble
	helperChunk = getChunk(chunkX - 1, chunkZ);
	if (helperChunk != nullptr) nibbles.push_back(helperChunk->getNibble(nibbleY));

	// +z nibble
	helperChunk = getChunk(chunkX, chunkZ + 1);
	if (helperChunk != nullptr) nibbles.push_back(helperChunk->getNibble(nibbleY));

	// +x nibble
	helperChunk = getChunk(chunkX, chunkZ - 1);
	if (helperChunk != nullptr) nibbles.push_back(helperChunk->getNibble(nibbleY));

	return nibbles;
}

void ChunkHandler::buildChunkNibbleBorders(chunkNibble* nibble)
{

}