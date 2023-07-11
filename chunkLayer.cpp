#include "chunkLayer.h"

chunkLayer::chunkLayer()
{
	//initializeLayerData();
}

void chunkLayer::initializeLayerData()
{
	layerData = new char[256];
	for (int x = 0; x < 16; x++)
		for (int y = 0; y < 16; y++)
			layerData[(y * 16) + x] = 0;
}