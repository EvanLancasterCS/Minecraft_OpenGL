#pragma once
#include <iostream>
#include <vector>

class chunkLayer
{
public:
	~chunkLayer()
	{
		if (layerData != nullptr) {
			free(layerData);
		}
	}
	chunkLayer();

	void setLayerBlock(int x, int z, int data) {
		//if (!inBounds(x, z))
		//	printf("ERROR: setting block out of layer bounds");

		if (layerData == nullptr) {
			if (data == 0)
				return;

			initializeLayerData();

		}
		int currentBlock = layerData[(z * 16) + x];
		if (currentBlock == 0 && data != 0) {
			numBlocksPlaced += 1;
			if (x == 0 || x == 15 || z == 0 || z == 15)
				numBlocksEdge += 1;
		}
		else if (currentBlock != 0 && data == 0)
		{
			numBlocksPlaced -= 1;
			if (x == 0 || x == 15 || z == 0 || z == 15)
				numBlocksEdge -= 1;
		}

		layerData[(z * 16) + x] = data;
	}
	int getLayerBlock(int x, int z)
	{
		//if (!inBounds(x, z))
		//	printf("ERROR: getting block out of layer bounds");

		if (layerData == nullptr)
			return 0;

		return layerData[(z * 16) + x];
	}
	bool isBlockAir(int x, int z) { return getLayerBlock(x, z) == 0; }
	bool isLayerEmpty() { return numBlocksPlaced == 0; }
	bool isLayerFull() { return numBlocksPlaced == 256; }
	bool hasBlocksAtEdge() { return numBlocksEdge != 0; }


	char* layerData = nullptr;
private:
	void initializeLayerData();
	bool inBounds(int x, int z) { return x >= 0 && x < 16 && z >= 0 && z < 16; }

	int numBlocksPlaced = 0;
	int numBlocksEdge = 0;
};