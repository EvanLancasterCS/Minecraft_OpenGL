#pragma once

const int SIDE_COLORS[6] = { 10, 5, 6, 6, 8, 5 };

const int BLOCK_ID_FACE_TEXTURES[][6] =
{
	{-1, -1, -1, -1, -1, -1}, // air
	{0, 0, 0, 0, 0, 0}, // dirt
	{2, 0, 1, 1, 1, 1}, // grass
	{3, 3, 3, 3, 3, 3}, // sand
	{4, 4, 4, 4, 4, 4}, // water
};

const int BLOCK_OPAQUE[] =
{
	0,
	1,
	1,
	1,
	0
};

// Refers to BLOCK_OFFSET_TYPE_INFO in the mesh generator for vertex offsets
const int BLOCK_OFFSET_TYPE[] =
{
	0,
	0,
	0,
	0,
	1
};




// Used for simplifying block offsets into "block offset types"
// 0 is regular block
// 1 is water
const float BLOCK_OFFSET_TYPE_INFO[][72] =
{
	// 0, regular block, no offsets
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Top Face
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Bottom Face
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Left Face
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Right Face
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Front Face
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // Back Face
	},
	// 1, water block
	{
		0, -0.1, 0, 0, -0.1, 0, 0, -0.1, 0, 0, -0.1, 0, // Top Face
		0, 0, 0, 0, 0, 0, 0, -0.1, 0, 0, -0.1, 0, // Bottom Face
		0, 0, 0, 0, 0, 0, 0, -0.1, 0, 0, -0.1, 0, // Left Face
		0, 0, 0, 0, 0, 0, 0, -0.1, 0, 0, -0.1, 0, // Right Face
		0, 0, 0, 0, 0, 0, -0.1, 0, 0, -0.1, 0, // Front Face
		0, 0, 0, 0, 0, 0, -0.1, 0, 0, -0.1, 0  // Back Face
	},
};