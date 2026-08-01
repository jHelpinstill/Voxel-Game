#pragma once
#ifndef CHUNK_MANAGER
#define CHUNK_MANAGER

#include "Chunk.h"

#define COORD_BITS 5
#define FACE_BITS 3
#define COLOR_BITS 4

class ChunkManager {
public:
	// Keys are used to distinguish different chunks within unordered maps by using their coordinates 

	struct DrawParams {
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
	Chunk::ShaderInfo shader_info = {COORD_BITS, FACE_BITS, COLOR_BITS}; // coord_bits, face_bits, color_bits
	int chunk_pos_bits = 10;
	float unit_length = 0.1;

	std::unordered_map<Chunk::Key, Chunk*> chunks;
	std::vector<int> pos_data;
	std::vector<DrawParams> draw_params;

	BVH<Chunk*> bvh;
	static void expandToFitChunk(const glm::vec3 &pos, Chunk **chunk, glm::vec3 &min, glm::vec3 &max);

	struct RaycastResult {
		bool hit;
		Chunk *chunk;
		BlockType *block;
		int face;
		glm::vec3 pos;
	};
	RaycastResult raycast(const glm::vec3 &pos, const glm::vec3 &ray);

	ChunkManager() : bvh(expandToFitChunk, 1) {}

	Chunk* get(const glm::vec3 &pos);
	Chunk* get(int x, int y, int z);
	bool add(int x, int y, int z);

	int size();

	Chunk* getNeighbor(Chunk *chunk, int face);
	Chunk::Group getNeighbors(Chunk *chunk);
};

#endif