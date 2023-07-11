#pragma once
#include "block_info.h"
#include <unordered_map>
#include "chunkHandler.h"
#include "chunkNibbleMeshData.h"
#include <future>
#include "ctpl.h"

// Creates basic mesh data for chunks
// This functionality is separated with the intention of making multithreading easier and less prone to bugs
static class ChunkMeshGenerator
{
public:
    enum class Cube_Faces { Top, Bottom, Left, Right, Front, Back };

    using position = std::pair<std::pair<int, int>, int>;
    struct tripair_hash;

    static void QueueMeshData(int chunkX, int nibbleY, int chunkZ);

    static void MeshUpdateTick(int playerX, int playerY, int playerZ);

    static bool IsMeshDone(int chunkX, int nibbleY, int chunkZ);

    static bool Working();

    // Automatically removes from map
    static ChunkNibbleMeshData& RetrieveMeshData(int chunkX, int nibbleY, int chunkZ);

    static void RemoveDataFromMap(int chunkX, int nibbleY, int chunkZ);

private:
    static position workingData;
    static std::unordered_map<position, ChunkNibbleMeshData, tripair_hash> data_map;
    static std::unordered_map<position, ChunkNibbleMeshData, tripair_hash> finished_data_map;
    static std::future<void> work_thread;
    static ctpl::thread_pool thread_pool;

    static position makeMapKey(int chunkX, int nibbleY, int chunkZ) { return position(std::pair<int, int>(chunkX, nibbleY), chunkZ); }

    static void RemoveDataFromMap(position key);

    static void ResetWorkingData() { workingData = position(std::pair<int, int>(INT_MAX, INT_MAX), INT_MAX); }
    static bool WorkingDataValid() { return workingData.first.first != INT_MAX; }

    // Given chunk layers, creates face information for inner chunk faces
    // "int id" is for multithreading library
    static void CreateInsideFaces(int id, int cx, int cy, int cz);

    static ChunkNibbleMeshData& GetClosestNibbleToPlayer(int playerX, int playerY, int playerZ);

    static bool isBlockAir(chunkLayer* layers, int x, int y, int z) { return layers[y].isBlockAir(x, z); }
    static bool isBlockInBounds(int x, int y, int z) { return x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16; }
    static bool isAirAndInBounds(chunkLayer* layers, int x, int y, int z) { return isBlockInBounds(x, y, z) && isBlockAir(layers, x, y, z); }
    static bool isBlockOpaque(chunkLayer* layers, int x, int y, int z) { return BLOCK_OPAQUE[layers[y].getLayerBlock(x, z)]; }
    static bool isSideRenderable(chunkLayer* layers, int chunkX, int chunkY, int chunkZ, int x, int y, int z, int xMod, int yMod, int zMod);
    static bool getNibbleBlock(chunkLayer* layers, int x, int y, int z) { return layers[y].getLayerBlock(x, z); }

    static int push_face_indices(Cube_Faces face, ChunkNibbleMeshData& package, float xPos, float yPos, float zPos, int block_id, std::vector<int>& verts_d1, std::vector<int>& verts_d2, std::vector<int>& indices);
    static int push_face_indices(Cube_Faces face, ChunkNibbleMeshData& package, float xPos, float yPos, float zPos, int block_id, std::vector<int>& verts_d1, std::vector<int>& verts_d2, std::vector<int>& indices,
        std::vector<int>& verts_d1_transparent, std::vector<int>& verts_d2_transparent, std::vector<int>& indices_transparent);

    static int getAmbientOccData(glm::vec3 normal, ChunkNibbleMeshData& package, int vert_raw, int x, int y, int z);

    // returns shifted input
    static int bitshiftInHelp(int input, int type);

    // retrieves value from vert_raw
    static int bitshiftOutHelp(int vertex, int type);

    static int bitshiftInHelp(int x, int y, int z)
    {
        return bitshiftInHelp(x, 0) + bitshiftInHelp(y, 1) + bitshiftInHelp(z, 2);
    }

    static bool thread_is_ready()
    {
        return work_thread.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    static bool thread_readable_flag;
    static ChunkNibbleMeshData thread_data;
};