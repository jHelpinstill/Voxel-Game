#pragma once
#ifndef CHUNK_
#define CHUNK_

#include "config.h"
#include "BlockType.h"

#define CHUNK_SIZE 32

class Chunk
{
public:
	BlockType blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
	glm::ivec3 pos;

	Chunk(glm::ivec3 pos);
};

struct ChunkKey
{
	int x, y, z;
	ChunkKey(int x, int y, int z) : x(x), y(y), z(z) {}
	bool operator==(const ChunkKey& other) const
	{
		return (x == other.x
			&& y == other.y
			&& z == other.z);
	}
};

template <>
struct std::hash<ChunkKey>
{
	std::size_t operator()(const ChunkKey& k) const
	{
		using std::size_t;
		using std::hash;
		using std::string;

		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:

		return ((hash<int>()(k.x)
			^ (hash<int>()(k.y) << 1)) >> 1)
			^ (hash<int>()(k.z) << 1);
	}
};

#endif