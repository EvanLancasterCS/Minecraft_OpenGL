#pragma once
#include "boxCollider.h"

class Entity
{
public:
	Entity() {
		collider = BoxCollider(this, 1, 1, 1);

	}

	// Returns 0-1 based on when the collision occured 
	float sweptAABB(BoxCollider other, float& normalX, float& normalY, float& normalZ);

private:


	BoxCollider collider;
};