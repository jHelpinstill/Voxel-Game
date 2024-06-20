#pragma once
#ifndef WORLD_
#define WORLD_

#include "config.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Chunk.h"

struct ChunkDrawParams
{
	unsigned int count;
	unsigned int instanceCount;
	unsigned int first;
	unsigned int baseInstance;

	ChunkDrawParams(
		unsigned int count,
		unsigned int instanceCount,
		unsigned int first,
		unsigned int baseInstance
	) : count(count), instanceCount(instanceCount), first(first), baseInstance(baseInstance) {}
};

class World
{
private:

public:
	float chunk_unit_length = 0.1;
	int chunk_radius = 6;
	long seed;

	float ambient_lighting = 0.1;
	glm::vec3 sun_dir = glm::vec3(-1);

	std::unordered_map<ChunkKey, Chunk*> chunks;
	std::vector<int> chunk_pos_data;
	std::vector<ChunkDrawParams> chunk_draw_params;

	World(long seed = 0);

	void setup();
	void generateMesh();
	void remeshChunk(Chunk* chunk);

	void addChunk(int x, int y, int z);
	Chunk* getChunk(int x, int y, int z);
	bool peekChunk(int x, int y, int z, Chunk** chunk_at = nullptr);

	void inspectPos(glm::vec3 pos, BlockType** block_at, Chunk** chunk_at = nullptr);
	BlockType* inspectPos(glm::vec3 pos);
	bool inspectRay(glm::vec3 pos, glm::vec3 dir, BlockType** block_at, Chunk** chunk_at = nullptr);
	BlockType* inspectRay(glm::vec3 pos, glm::vec3 dir);

	void updateBlock(glm::vec3 pos, BlockType new_type);
	void updateLookedAtBlock(Camera* camera, BlockType new_type);

	int encodeChunkPos(Chunk* chunk);
	static void drawWorld(Mesh* mesh, Camera* camera, void* obj);
};

#endif