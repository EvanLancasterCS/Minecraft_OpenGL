#pragma once
#include <vector>
#include <unordered_map>

// Package of mesh data to be retrieved by the nibbles
class ChunkNibbleMeshData
{
public:
    enum class Stage
    {
        Waiting,
        Generating,
        Done
    };

    std::vector<int> verts_d1;
    std::vector<int> verts_d2;
    std::vector<int> indices;

    std::vector<int> verts_d1_transparent;
    std::vector<int> verts_d2_transparent;
    std::vector<int> indices_transparent;

    using position = std::pair<std::pair<int, int>, int>;
    struct tripair_hash
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
    std::unordered_map<position, bool, tripair_hash> colliderData;

    int chunkX;
    int nibbleY;
    int chunkZ;

    double layerDataTimestamp;

    Stage currentStage;

    bool valid = true;

    ChunkNibbleMeshData(int chunkX, int nibbleY, int chunkZ, bool valid = true)
    {
        this->chunkX = chunkX;
        this->nibbleY = nibbleY;
        this->chunkZ = chunkZ;
        this->valid = valid;

        currentStage = Stage::Waiting;
    }

    void replace(ChunkNibbleMeshData other)
    {
        this->verts_d1 = other.verts_d1;
        this->verts_d2 = other.verts_d2;
        this->indices = other.indices;

        this->verts_d1_transparent = other.verts_d1_transparent;
        this->verts_d2_transparent = other.verts_d2_transparent;
        this->indices_transparent = other.indices_transparent;

        this->chunkX = other.chunkX;
        this->nibbleY = other.nibbleY;
        this->chunkZ = other.chunkZ;
        this->currentStage = other.currentStage;
        this->layerDataTimestamp = other.layerDataTimestamp;
        this->colliderData = other.colliderData;
    }

    void tryInsertColliderData(int x, int y, int z)
    {
        position p = std::pair<std::pair<int, int>, int>(std::pair<int,int>(x, y), z);
        if (colliderData.find(p) == colliderData.end())
            colliderData.insert({ p, true });
    }

    void setStage(Stage stage) { currentStage = stage; }

    bool isDone() { return currentStage == Stage::Done; }
};