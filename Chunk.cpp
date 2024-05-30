#include "Chunk.h"

Chunk::Chunk(glm::ivec3 pos)
{
	this->pos = pos;

	//for (int y = 0; y < CHUNK_SIZE / 2; y++)
	//	for (int z = 0; z < CHUNK_SIZE; z++)
	//		for (int x = 0; x < CHUNK_SIZE; x++)
	//			blocks[x][y][z] = BlockType::DIRT;
	//
	//for (int y = CHUNK_SIZE / 2; y < CHUNK_SIZE; y++)
	//	for (int z = 0; z < CHUNK_SIZE; z++)
	//		for (int x = 0; x < CHUNK_SIZE; x++)
	//			blocks[x][y][z] = BlockType::AIR;

	for (int x = 0; x < CHUNK_SIZE; x++) for (int z = 0; z < CHUNK_SIZE; z++) for (int y = 0; y < CHUNK_SIZE; y++)
	{
		if (y < (2 * (sin(x * 6.0 / CHUNK_SIZE) * cos(z * 6.0 / CHUNK_SIZE)) + (CHUNK_SIZE / 2)))
			blocks[x][y][z] = BlockType::DIRT;
		else
			blocks[x][y][z] = BlockType::AIR;
	}
}