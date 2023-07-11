#pragma once
#include "debug_tools.h"

class Entity;

class BoxCollider
{
public:
	
	// Static world colliders
	BoxCollider(float xPos, float yPos, float zPos, float sizeX, float sizeY, float sizeZ);
	BoxCollider(float xPos, float yPos, float zPos) : BoxCollider(xPos, yPos, zPos, 1, 1, 1) {}

	// Dynamic colliders
	BoxCollider(Entity* entity, float sizeX, float sizeY, float sizeZ);

	bool isPointInside(float x, float y, float z);

	bool isColliding(BoxCollider other);
	
	void addDebugVisual() { Debug_Tools::CreateWireframe(xPos, yPos, zPos, sizeX, sizeY, sizeZ); }

	void setEntityReference(Entity* entity) { this->entity = entity; }

private:
	
	Entity* entity = nullptr;

	void setBounds() 
	{ 
		minX = xPos - sizeX / 2; 
		maxX = xPos + sizeX / 2;
		minY = yPos - sizeY / 2;
		maxY = yPos + sizeY / 2;
		minZ = zPos - sizeZ / 2;
		maxZ = zPos + sizeZ / 2;
	}

	// bounds of the collider
	float minX, maxX, minY, maxY, minZ, maxZ;

	// Size
	float sizeX, sizeY, sizeZ;

	// Position
	float xPos, yPos, zPos;
};