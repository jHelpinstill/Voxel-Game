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

	unsigned int pos_SSBO = 0;
	float unit_length = 0.1;

	std::unordered_map<Chunk::Key, Chunk*> chunks;
	std::vector<int> pos_data;
	std::vector<DrawParams> draw_params;

	BVH<Chunk*> bvh;
	static bool raycastChunk(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3& chunk_pos, Chunk** chunk);
	static void expandToFitChunk(const glm::vec3& pos, Chunk** chunk, glm::vec3& min, glm::vec3& max);

	struct RaycastResult
	{
		bool hit;
		Chunk* chunk;
		BlockType* block;
		int face;
		glm::vec3 pos;
	};
	RaycastResult raycast(const glm::vec3& pos, const glm::vec3& ray);

	ChunkManager() : bvh(raycastChunk, expandToFitChunk, 1) {}

	Chunk* get(const glm::vec3& pos);
	Chunk* get(int x, int y, int z);
	bool add(int x, int y, int z);

	int size();

	Chunk* getNeighbor(Chunk* chunk, int face);
	Chunk::Group getNeighbors(Chunk* chunk);
};

#endif