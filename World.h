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
	static float chunk_unit_length;
	int chunk_radius = 5;

	std::unordered_map<ChunkKey, Chunk*> chunks;
	static std::vector<int> chunk_pos_data;
	static std::vector<ChunkDrawParams> chunk_draw_params;

	World();

	void setup();
	void generateMesh();

	void addChunk(int x, int y, int z);
	Chunk* getChunk(int x, int y, int z);

	void inspectPos(glm::vec3 pos, BlockType** block_at, Chunk** chunk_at = nullptr);
	void updateBlock(glm::vec3 pos, BlockType new_type);

	int encodeChunkPos(Chunk* chunk);
	static void drawWorld(Mesh* mesh, Camera* camera);
};

#endif