#include "Chunk.h"

Chunk::Chunk(glm::ivec3 pos)
{
	this->pos = pos;

	for (int z = 0; z < CHUNK_SIZE / 2; z++)
		for (int y = 0; y < CHUNK_SIZE; y++)
			for (int x = 0; x < CHUNK_SIZE; x++)
				blocks[x][y][z] = BlockType::DIRT;

	for (int z = CHUNK_SIZE / 2; z < CHUNK_SIZE; z++)
		for (int y = 0; y < CHUNK_SIZE; y++)
			for (int x = 0; x < CHUNK_SIZE; x++)
				blocks[x][y][z] = BlockType::AIR;
}