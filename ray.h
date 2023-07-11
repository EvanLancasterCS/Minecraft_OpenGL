#pragma once
#include "chunkHandler.h"

class RayHitInfo
{
public:
	RayHitInfo(bool hit, int blockX, int blockY, int blockZ, float hitX, float hitY, float hitZ, int normalX, int normalY, int normalZ)
	{
		this->hit = hit;
		this->blockX = blockX;
		this->blockY = blockY;
		this->blockZ = blockZ;
		this->hitX = hitX;
		this->hitY = hitY;
		this->hitZ = hitZ;
		this->hitNormalX = normalX;
		this->hitNormalY = normalY;
		this->hitNormalZ = normalZ;
	}
	RayHitInfo(bool hit) : RayHitInfo(hit, -1, -1, -1, -1, -1, -1, -1, -1, -1) {}

	bool hit = false;
	int blockX, blockY, blockZ;
	float hitX, hitY, hitZ;
	int hitNormalX, hitNormalY, hitNormalZ;
};

class Ray
{
public:
	Ray(float startX, float startY, float startZ, float dirX, float dirY, float dirZ, float length)
	{
		this->startX = startX;
		this->startY = startY;
		this->startZ = startZ;

		this->dirX = dirX;
		this->dirY = dirY;
		this->dirZ = dirZ;

		this->length = length;
	}

	// Help from https://www.youtube.com/watch?v=NbSee-XM7WA
	// Works by checking every integer that it hits as it moves along the ray
	RayHitInfo Raycast()
	{
		float rayUnitStepSizeX, rayUnitStepSizeY, rayUnitStepSizeZ, rayLength1Dx, rayLength1Dy, rayLength1Dz;
		int tileX, tileY, tileZ, stepX, stepY, stepZ;
		bool nX = false, nY = true, nZ = false;

		RaycastHelper(tileX, rayUnitStepSizeX, rayLength1Dx, stepX, dirX, dirY, dirZ, startX);
		RaycastHelper(tileY, rayUnitStepSizeY, rayLength1Dy, stepY, dirY, dirX, dirZ, startY);
		RaycastHelper(tileZ, rayUnitStepSizeZ, rayLength1Dz, stepZ, dirZ, dirY, dirX, startZ);


		if (length > 0)
		{
			float distance = 0.0f;
			while ((distance * distance) < (length * length))
			{
				// Successful raycast
				if (PositionHasBlock(tileX, tileY, tileZ))
				{
					int normalX = nX ? -stepX : 0;
					int normalY = nY ? -stepY : 0;
					int normalZ = nZ ? -stepZ : 0;

					return RayHitInfo(true,
						tileX, tileY, tileZ,
						0, 0, 0,
						normalX, normalY, normalZ
					);

				}

				if (rayLength1Dx < rayLength1Dy && rayLength1Dx < rayLength1Dz)
				{
					tileX += stepX;
					distance = rayLength1Dx;
					rayLength1Dx += rayUnitStepSizeX;
					nX = true; nY = false; nZ = false;
				}
				else if (rayLength1Dy < rayLength1Dx && rayLength1Dy < rayLength1Dz)
				{
					tileY += stepY;
					distance = rayLength1Dy;
					rayLength1Dy += rayUnitStepSizeY;
					nX = false; nY = true; nZ = false;
				}
				else
				{
					tileZ += stepZ;
					distance = rayLength1Dz;
					rayLength1Dz += rayUnitStepSizeZ;
					nX = false; nY = false; nZ = true;
				}
			}
		}
		return RayHitInfo(false);
	}

private:

	void RaycastHelper(int& tile, float& rayUnitStepSize, float& rayLength1D, int& step, float mainDir, float subDir1, float subDir2, float start)
	{
		rayUnitStepSize = sqrt(1 + pow(subDir1 / mainDir, 2) + pow(subDir2 / mainDir, 2));
		tile = floor(start);

		if (mainDir < 0)
		{
			step = -1;
			rayLength1D = (start - float(tile)) * rayUnitStepSize;
		}
		else
		{
			step = 1;
			rayLength1D = (float(tile + 1) - start) * rayUnitStepSize;
		}
	}

	float GetSquaredDistance(float currX, float currY, float currZ) { return pow(currX - startX, 2) + pow(currY - startY, 2) + pow(currZ - startZ, 2); }
	bool PositionHasBlock(int x, int y, int z) { return ChunkHandler::instance->isBlockAt(x, y, z); }

	float startX, startY, startZ;
	float dirX, dirY, dirZ;
	float length;
};