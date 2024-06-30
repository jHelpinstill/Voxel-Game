#pragma once
#ifndef CHUNK_MANAGER
#define CHUNK_MANAGER

#include "Chunk.h"

class ChunkManager
{
public:
	// Keys are used to distinguish different chunks within unordered maps by using their coordinates 

	struct DrawParams
	{
		unsigned int count;
		unsigned int instanceCount;
		unsigned int first;
		unsigned int baseInstance;

		DrawParams(
			unsigned int count,
			unsigned int instanceCount,
			unsigned int first,
			unsigned int baseInstance
		) : count(count), instanceCount(instanceCount), first(first), baseInstance(baseInstance) {}
	};

	unsigned int pos_SSBO;
	float unit_length = 0.1;

	std::unordered_map<Chunk::Key, Chunk*> chunks;
	std::vector<int> pos_data;
	std::vector<DrawParams> draw_params;

	BVH<Chunk*> bvh;

	ChunkManager() {}

	Chunk* get(const glm::vec3& pos);
	Chunk* get(int x, int y, int z);
	void add(int x, int y, int z);

	int size();

	Chunk::Group getNeighbors(Chunk* chunk);
};

#endif