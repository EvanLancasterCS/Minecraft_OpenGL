#include "boxCollider.h"

BoxCollider::BoxCollider(float xPos, float yPos, float zPos, float sizeX, float sizeY, float sizeZ)
{
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->sizeZ = sizeZ;
	this->xPos = xPos;
	this->yPos = yPos;
	this->zPos = zPos;

	setBounds();
}

BoxCollider::BoxCollider(Entity* entity, float sizeX, float sizeY, float sizeZ)
{
	setEntityReference(entity);
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->sizeZ = sizeZ;
	this->xPos = xPos;
	this->yPos = yPos;
	this->zPos = zPos;

	setBounds();
}

bool BoxCollider::isPointInside(float x, float y, float z)
{
	return (x > minX && x < maxX &&
			y > minY && y < minY &&
			z > minZ && z < minZ   );
}

bool BoxCollider::isColliding(BoxCollider other)
{
	//todo
	return false;
}