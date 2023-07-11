#include "chunkMeshGenerator.h"

struct ChunkMeshGenerator::tripair_hash
{
    int cantor(const int a, const int b) const { return (a + b + 1) * (a + b) / 2 + b; }

    template <class T1, class T2, class T3>
    std::size_t operator() (const std::pair<std::pair<T1, T2>, T3>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first.first);
        auto h2 = std::hash<T2>{}(p.first.second);
        auto h3 = std::hash<T3>{}(p.second);
        return cantor(h1, cantor(h2, h3));
    }
};

std::unordered_map<ChunkMeshGenerator::position, ChunkNibbleMeshData, ChunkMeshGenerator::tripair_hash> ChunkMeshGenerator::data_map;
std::unordered_map<ChunkMeshGenerator::position, ChunkNibbleMeshData, ChunkMeshGenerator::tripair_hash> ChunkMeshGenerator::finished_data_map;
std::future<void> ChunkMeshGenerator::work_thread;
ChunkMeshGenerator::position ChunkMeshGenerator::workingData = ChunkMeshGenerator::position(std::pair<int, int>(INT_MAX, INT_MAX), INT_MAX);;
bool ChunkMeshGenerator::thread_readable_flag = false;
ChunkNibbleMeshData ChunkMeshGenerator::thread_data = ChunkNibbleMeshData(0, 0, 0);

ctpl::thread_pool ChunkMeshGenerator::thread_pool(1);


void ChunkMeshGenerator::QueueMeshData(int chunkX, int nibbleY, int chunkZ)
{
    position key = makeMapKey(chunkX, nibbleY, chunkZ);
    if (data_map.find(key) != data_map.end())
    {
        // Key already exists, do something?
    }
    else
    {
        ChunkNibbleMeshData newData = ChunkNibbleMeshData(chunkX, nibbleY, chunkZ);
        data_map.insert({ key, newData });
    }
}

bool ChunkMeshGenerator::Working()
{
    if (!WorkingDataValid()) return false;

    if (data_map.find(workingData) == data_map.end())
    {
        ResetWorkingData();
        return false;
    }

    if (data_map.at(workingData).currentStage == ChunkNibbleMeshData::Stage::Done)
    {
        ResetWorkingData();
        return false;
    }

    return true;
}

void ChunkMeshGenerator::MeshUpdateTick(int playerX, int playerY, int playerZ)
{
    // If we aren't working, look for a mesh to generate.
    // If we are working, wait until we see that the thread is finished and push it to be available to the nibbles.
    if (!Working())
    {
        if (data_map.size() == 0) return;
        ChunkNibbleMeshData& meshData = GetClosestNibbleToPlayer(playerX, playerY, playerZ);
        if (!meshData.valid) return;

        int cX = meshData.chunkX;
        int cY = meshData.nibbleY;
        int cZ = meshData.chunkZ;
            
        Chunk* chunk = ChunkHandler::instance->getChunk(cX, cZ);
        if (chunk != nullptr) {
            meshData.currentStage = ChunkNibbleMeshData::Stage::Generating;
            chunkNibble* nibble = chunk->getNibble(cY);
            chunkLayer* layers = nibble->getNibbleLayers();

            thread_readable_flag = false;
            
            // Tell the thread to begin work on mesh
            thread_pool.push(CreateInsideFaces, cX, cY, cZ);

            workingData = makeMapKey(cX, cY, cZ);

            return;
        }
        else
        {
            RemoveDataFromMap(makeMapKey(cX, cY, cZ));
            return;
        }

    }
    else
    {
        if (thread_readable_flag)
        {
            data_map.at(workingData).replace(thread_data);
            thread_readable_flag = false;
            ResetWorkingData();
        }
    }
}

bool ChunkMeshGenerator::IsMeshDone(int chunkX, int nibbleY, int chunkZ)
{
    position key = makeMapKey(chunkX, nibbleY, chunkZ);
    if (data_map.find(key) != data_map.end())
    {
        return data_map.at(key).isDone();
    }
    std::cout << "Warning; chunk is looking for a mesh that isn't in queue\n";
    return false;
}

ChunkNibbleMeshData& ChunkMeshGenerator::RetrieveMeshData(int chunkX, int nibbleY, int chunkZ)
{
    if (IsMeshDone(chunkX, nibbleY, chunkZ))
    {
        position key = makeMapKey(chunkX, nibbleY, chunkZ);
        ChunkNibbleMeshData& meshData = data_map.at(key);
        return meshData;
    }
    return data_map.at(makeMapKey(chunkX, nibbleY, chunkZ));
}

void ChunkMeshGenerator::RemoveDataFromMap(position key)
{
    if (data_map.find(key) != data_map.end())
    {
        data_map.erase(key);
    }
}

void ChunkMeshGenerator::RemoveDataFromMap(int chunkX, int nibbleY, int chunkZ)
{
    position key = makeMapKey(chunkX, nibbleY, chunkZ);
    RemoveDataFromMap(key);
}

ChunkNibbleMeshData& ChunkMeshGenerator::GetClosestNibbleToPlayer(int playerX, int playerY, int playerZ)
{
    int pChunkX = playerX / 16;
    int pChunkY = playerY / 16;
    int pChunkZ = playerZ / 16;

    auto iterator = data_map.begin();
    ChunkNibbleMeshData closest = ChunkNibbleMeshData(0, 0, 0, false);
    
    for (auto& data : data_map)
    {
        if (data.second.currentStage != ChunkNibbleMeshData::Stage::Waiting) {
            continue;
        }

        position pos = data.first;
        int chunkX = pos.first.first;
        int chunkY = pos.first.second;
        int chunkZ = pos.second;

        int distance1 = pow(closest.chunkX - pChunkX, 2) + pow(closest.nibbleY - pChunkY, 1) + pow(closest.chunkZ - pChunkZ, 2);
        int distance2 = pow(chunkX - pChunkX, 2) + pow(chunkY - pChunkY, 1) + pow(chunkZ - pChunkZ, 2);
        if (distance2 < distance1 || !closest.valid)
        {
            closest = data.second;
        }
    }
    return closest;
}

// Given chunk layers, creates face information for inner chunk faces
void ChunkMeshGenerator::CreateInsideFaces(int id, int cx, int cy, int cz)//ChunkNibbleMeshData* meshData)
{
    position key = makeMapKey(cx, cy, cz);
    if (data_map.find(key) == data_map.end()) return;
    ChunkNibbleMeshData meshData = data_map.at(key);

    // Get location of all of the needed meshdata variables
    std::vector<int>& verts_d1 = meshData.verts_d1;
    std::vector<int>& verts_d2 = meshData.verts_d2;
    std::vector<int>& indices = meshData.indices;
    std::vector<int>& verts_d1_transparent = meshData.verts_d1_transparent;
    std::vector<int>& verts_d2_transparent = meshData.verts_d2_transparent;
    std::vector<int>& indices_transparent = meshData.indices_transparent;

    // Null checks because we are expecting to be on a separate thread
    Chunk* chunk = ChunkHandler::instance->getChunk(meshData.chunkX, meshData.chunkZ); 
    if (chunk == nullptr) return;
    chunkNibble* nibble = chunk->getNibble(meshData.nibbleY);                          
    if (nibble == nullptr) return;
    chunkLayer* chunkLayers = nibble->getNibbleLayers();                               
    if (chunkLayers == nullptr) return;
    meshData.layerDataTimestamp = glfwGetTime();

    bool playerInChunk = ChunkHandler::instance->isPlayerInChunk(cx, cy, cz);

    std::vector<int>* verts_d1_choice = &verts_d1;
    std::vector<int>* verts_d2_choice = &verts_d2;
    std::vector<int>* indices_choice = &indices;

    // Loop all faces
    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            for (int z = 0; z < 16; z++)
            {
                if (!isBlockAir(chunkLayers, x, y, z)) {
                    int block = chunkLayers[y].getLayerBlock(x, z);

                    // Separate transparent and opaque meshes into different geometry
                    if (BLOCK_OPAQUE[block])
                    {
                        verts_d1_choice = &verts_d1;
                        verts_d2_choice = &verts_d2;
                        indices_choice = &indices;
                    }
                    else
                    {
                        verts_d1_choice = &verts_d1_transparent;
                        verts_d2_choice = &verts_d2_transparent;
                        indices_choice = &indices_transparent;
                    }

                        
                    if (isSideRenderable(chunkLayers, cx, cy, cz, x, y, z, x + 1, y + 0, z + 0)) push_face_indices(Cube_Faces::Right, meshData, x, y, z, block, *verts_d1_choice, *verts_d2_choice, *indices_choice);
                    if (isSideRenderable(chunkLayers, cx, cy, cz, x, y, z, x - 1, y + 0, z + 0)) push_face_indices(Cube_Faces::Left, meshData, x, y, z, block, *verts_d1_choice, *verts_d2_choice, *indices_choice);
                    if (isSideRenderable(chunkLayers, cx, cy, cz, x, y, z, x + 0, y + 1, z + 0)) push_face_indices(Cube_Faces::Top, meshData, x, y, z, block, *verts_d1_choice, *verts_d2_choice, *indices_choice);
                    if (isSideRenderable(chunkLayers, cx, cy, cz, x, y, z, x + 0, y - 1, z + 0)) push_face_indices(Cube_Faces::Bottom, meshData, x, y, z, block, *verts_d1_choice, *verts_d2_choice, *indices_choice);
                    if (isSideRenderable(chunkLayers, cx, cy, cz, x, y, z, x + 0, y + 0, z + 1)) push_face_indices(Cube_Faces::Front, meshData, x, y, z, block, *verts_d1_choice, *verts_d2_choice, *indices_choice);
                    if (isSideRenderable(chunkLayers, cx, cy, cz, x, y, z, x + 0, y + 0, z - 1)) push_face_indices(Cube_Faces::Back, meshData, x, y, z, block, *verts_d1_choice, *verts_d2_choice, *indices_choice);
                }
            }
        }
    }

    meshData.setStage(ChunkNibbleMeshData::Stage::Done);
    if (data_map.find(key) == data_map.end()) return;

    thread_data = meshData;

    thread_readable_flag = true;
}

bool ChunkMeshGenerator::isSideRenderable(chunkLayer* layers, int chunkX, int chunkY, int chunkZ, int x, int y, int z, int xMod, int yMod, int zMod) {
    // Side faces towards the inside of the nibble mesh
    if (isBlockInBounds(xMod, yMod, zMod)) {
        return isBlockAir(layers, xMod, yMod, zMod) || (isBlockOpaque(layers, x, y, z) && !isBlockOpaque(layers, xMod, yMod, zMod));
    }

    // Side faces towards another nibble
    int realXMod = xMod + (chunkX * 16);
    int realYMod = yMod + (chunkY * 16);
    int realZMod = zMod + (chunkZ * 16);
    int blockID = ChunkHandler::instance->getBlockAt(realXMod, realYMod, realZMod);
    if (blockID == -1)
        return false;

    return blockID == 0 || ((isBlockOpaque(layers, x, y, z) && !BLOCK_OPAQUE[blockID]));
}

int ChunkMeshGenerator::bitshiftInHelp(int input, int type)
{
    // type0 = x, type1 = y, type2 = z, type3 = U, type4 = V, type5 = l, type6 = ambientocc
    switch (type)
    {
    case 0: //x
        return (31 << (32 - 5)) & (input << (32 - 5));
    case 1: //y
        return (511 << (32 - 14)) & (input << (32 - 14));
    case 2: //z
        return (31 << (32 - 19)) & (input << (32 - 19));
    case 3: //u
        return (31 << (32 - 24)) & (input << (32 - 24));
    case 4: //v
        return (31 << (32 - 29)) & (input << (32 - 29));
    case 5: //l
        return (15 << (32 - 4)) & (input << (32 - 4));
    case 6: //ambientOcc
        return (7 << (32 - 32)) & (input << (32 - 32));
    default:
        return -1;
    }
}

int ChunkMeshGenerator::bitshiftOutHelp(int vertex, int type)
{
    // type0 = x, type1 = y, type2 = z, type3 = U, type4 = V, type5 = l, type6 = ambientocc
    switch (type)
    {
    case 0: //x
        return 31 & (vertex >> (32 - 5));
    case 1: //y
        return 511 & (vertex >> (32 - 14));
    case 2: //z
        return 31 & (vertex >> (32 - 19));
    case 3: //u
        return 31 & (vertex >> (32 - 24));
    case 4: //v
        return 31 & (vertex >> (32 - 29));
    case 5: //l
        return 15 & (vertex >> (32 - 4));
    case 6: //ambientOcc
        return 7 & (vertex >> (32 - 32));
    default:
        return -1;
    }
}

int ChunkMeshGenerator::getAmbientOccData(glm::vec3 normal, ChunkNibbleMeshData& package, int vert_raw, int x, int y, int z)
{
    int vert_raw_xPos = bitshiftOutHelp(vert_raw, 0);
    int vert_raw_yPos = bitshiftOutHelp(vert_raw, 1);
    int vert_raw_zPos = bitshiftOutHelp(vert_raw, 2);
    int worldX = (package.chunkX * 16) + x;
    int worldY = (package.nibbleY * 16) + y;
    int worldZ = (package.chunkZ * 16) + z;

    if (!ChunkHandler::instance->isBlockOpaque(worldX, worldY, worldZ))
        return 3;

    if (normal.x != 0)
    {
        int yDir = (vert_raw_yPos > y) ? 1 : -1;
        int zDir = (vert_raw_zPos > z) ? 1 : -1;

        int side1 = ChunkHandler::instance->isBlockOpaque(worldX + normal.x, worldY + yDir, worldZ);
        int side2 = ChunkHandler::instance->isBlockOpaque(worldX + normal.x, worldY, worldZ + zDir);
        int corner = ChunkHandler::instance->isBlockOpaque(worldX + normal.x, worldY + yDir, worldZ + zDir);
        if (side1 && side2)
            return 0;
        else
            return 3 - (side1 + side2 + corner);
    }
    else if (normal.y != 0)
    {
        int xDir = (vert_raw_xPos > x) ? 1 : -1;
        int zDir = (vert_raw_zPos > z) ? 1 : -1;

        int side1 = ChunkHandler::instance->isBlockOpaque(worldX + xDir, worldY + normal.y, worldZ);
        int side2 = ChunkHandler::instance->isBlockOpaque(worldX, worldY + normal.y, worldZ + zDir);
        int corner = ChunkHandler::instance->isBlockOpaque(worldX + xDir, worldY + normal.y, worldZ + zDir);
        if (side1 && side2)
            return 0;
        else
            return 3 - (side1 + side2 + corner);
    }
    else if (normal.z != 0)
    {
        int xDir = (vert_raw_xPos > x) ? 1 : -1;
        int yDir = (vert_raw_yPos > y) ? 1 : -1;
        int side1 = ChunkHandler::instance->isBlockOpaque(worldX + xDir, worldY, worldZ + normal.z);
        int side2 = ChunkHandler::instance->isBlockOpaque(worldX, worldY + yDir, worldZ + normal.z);
        int corner = ChunkHandler::instance->isBlockOpaque(worldX + xDir, worldY + yDir, worldZ + normal.z);
        if (side1 && side2)
            return 0;
        else
            return 3 - (side1 + side2 + corner);
    }
    return 0;
}


int ChunkMeshGenerator::push_face_indices(Cube_Faces face, ChunkNibbleMeshData& package, float xPos, float yPos, float zPos, int block_id, std::vector<int>& verts_d1, std::vector<int>& verts_d2, std::vector<int>& indices)
{
    package.tryInsertColliderData(xPos, yPos, zPos);

    int i = verts_d1.size();
    glm::vec3 normal = glm::vec3(0, 1, 0);

    if (face == Cube_Faces::Top) {
        verts_d1.insert(verts_d1.end(), {
            bitshiftInHelp(xPos + 0, yPos + 1, zPos + 1),
            bitshiftInHelp(xPos + 1, yPos + 1, zPos + 1),
            bitshiftInHelp(xPos + 0, yPos + 1, zPos + 0),
            bitshiftInHelp(xPos + 1, yPos + 1, zPos + 0)
            });
        normal = glm::vec3(0, 1, 0);

        indices.insert(indices.end(), { i + 0, i + 1, i + 3, i + 0, i + 3, i + 2 });
    }
    else if (face == Cube_Faces::Bottom) {
        verts_d1.insert(verts_d1.end(), {
            bitshiftInHelp(xPos + 0, yPos + 0, zPos + 0),
            bitshiftInHelp(xPos + 1, yPos + 0, zPos + 0),
            bitshiftInHelp(xPos + 0, yPos + 0, zPos + 1),
            bitshiftInHelp(xPos + 1, yPos + 0, zPos + 1)
            });
        normal = glm::vec3(0, -1, 0);

        indices.insert(indices.end(), { i + 0, i + 1, i + 3, i + 0, i + 3, i + 2 });
    }
    else if (face == Cube_Faces::Left) {
        verts_d1.insert(verts_d1.end(), {
            bitshiftInHelp(xPos + 0, yPos + 0, zPos + 0), // 0
            bitshiftInHelp(xPos + 0, yPos + 0, zPos + 1), // 1
            bitshiftInHelp(xPos + 0, yPos + 1, zPos + 0), // 2
            bitshiftInHelp(xPos + 0, yPos + 1, zPos + 1) // 3
            });
        normal = glm::vec3(-1, 0, 0);

        indices.insert(indices.end(), { i + 0, i + 1, i + 3, i + 0, i + 3, i + 2 });
    }
    else if (face == Cube_Faces::Right) {
        verts_d1.insert(verts_d1.end(), {
            bitshiftInHelp(xPos + 1, yPos + 0, zPos + 1),
            bitshiftInHelp(xPos + 1, yPos + 0, zPos + 0),
            bitshiftInHelp(xPos + 1, yPos + 1, zPos + 1),
            bitshiftInHelp(xPos + 1, yPos + 1, zPos + 0)
            });
        normal = glm::vec3(1, 0, 0);

        indices.insert(indices.end(), { i + 0, i + 1, i + 3, i + 0, i + 3, i + 2 });
    }
    else if (face == Cube_Faces::Front) {
        verts_d1.insert(verts_d1.end(), {
            bitshiftInHelp(xPos + 0, yPos + 0, zPos + 1),
            bitshiftInHelp(xPos + 1, yPos + 0, zPos + 1),
            bitshiftInHelp(xPos + 0, yPos + 1, zPos + 1),
            bitshiftInHelp(xPos + 1, yPos + 1, zPos + 1)
            });
        normal = glm::vec3(0, 0, 1);

        indices.insert(indices.end(), { i + 0, i + 1, i + 3, i + 0, i + 3, i + 2 });
    }
    else if (face == Cube_Faces::Back) {
        verts_d1.insert(verts_d1.end(), { \
            bitshiftInHelp(xPos + 1, yPos + 0, zPos + 0), // 0
            bitshiftInHelp(xPos + 0, yPos + 0, zPos + 0), // 1
            bitshiftInHelp(xPos + 1, yPos + 1, zPos + 0), // 2
            bitshiftInHelp(xPos + 0, yPos + 1, zPos + 0)  // 3
            });

        normal = glm::vec3(0, 0, -1);

        indices.insert(indices.end(), { i + 0, i + 1, i + 3, i + 0, i + 3, i + 2 });
    }

    // Texture location calculation
    int textureNum = BLOCK_ID_FACE_TEXTURES[block_id][(int)face];
    
    int xInt = textureNum % 16;
    int yInt = textureNum / 16;

    verts_d1[i + 0] += bitshiftInHelp(xInt, 3);
    verts_d1[i + 0] += bitshiftInHelp((15 - yInt), 4);
    verts_d1[i + 1] += bitshiftInHelp(xInt + 1, 3);
    verts_d1[i + 1] += bitshiftInHelp((15 - yInt), 4);
    verts_d1[i + 2] += bitshiftInHelp(xInt, 3);
    verts_d1[i + 2] += bitshiftInHelp((15 - yInt) + 1, 4);
    verts_d1[i + 3] += bitshiftInHelp(xInt + 1, 3);
    verts_d1[i + 3] += bitshiftInHelp((15 - yInt) + 1, 4);

    // ambient occlusion calculation & quad flipping
    int aOccData0 = getAmbientOccData(normal, package, verts_d1[i + 0], xPos, yPos, zPos);
    int aOccData1 = getAmbientOccData(normal, package, verts_d1[i + 1], xPos, yPos, zPos);
    int aOccData2 = getAmbientOccData(normal, package, verts_d1[i + 2], xPos, yPos, zPos);
    int aOccData3 = getAmbientOccData(normal, package, verts_d1[i + 3], xPos, yPos, zPos);

    verts_d1[i + 0] += bitshiftInHelp(aOccData0, 6);
    verts_d1[i + 1] += bitshiftInHelp(aOccData1, 6);
    verts_d1[i + 2] += bitshiftInHelp(aOccData2, 6);
    verts_d1[i + 3] += bitshiftInHelp(aOccData3, 6);

    // brute force things that im not proud of
    bool condition = face == Cube_Faces::Back && aOccData2 + aOccData3 < aOccData0 + aOccData1;
    bool condition2 = face == Cube_Faces::Top && aOccData1 + aOccData2 > aOccData3 + aOccData0;
    bool condition3 = face == Cube_Faces::Top && (aOccData0 == 0 || aOccData3 == 0);
    bool condition4 = (aOccData1 + aOccData3 > aOccData2 + aOccData0 && face != Cube_Faces::Top);

    // flip quad to counter visual errors due to meshing
    if (condition || condition2 || condition3 || condition4)
    {
        int d10 = verts_d1[i + 0];
        int d11 = verts_d1[i + 1];
        int d12 = verts_d1[i + 2];
        int d13 = verts_d1[i + 3];
        verts_d1[i + 0] = d12;
        verts_d1[i + 1] = d10;
        verts_d1[i + 2] = d13;
        verts_d1[i + 3] = d11;
    }


    // Lighting to make world look "clearer"
    int lighting = SIDE_COLORS[(int)face];
    verts_d2.insert(verts_d2.end(), {
        bitshiftInHelp(lighting, 5),
        bitshiftInHelp(lighting, 5),
        bitshiftInHelp(lighting, 5),
        bitshiftInHelp(lighting, 5)
        }); // redundant rn but won't be later
        
    return 1;

}