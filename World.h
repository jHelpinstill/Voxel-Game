#pragma once
#ifndef WORLD_
#define WORLD_

#include "config.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Chunk.h"
#include "BVH.h"

class World
{
private:
	unsigned int chunk_pos_SSBO;
public:
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

	float chunk_unit_length = 0.1;
	int chunk_radius = 0;
	long seed;

	float ambient_lighting = 0.1;
	glm::vec3 sun_dir = glm::vec3(-1);

	std::unordered_map<Chunk::Key, Chunk*> chunks;
	std::vector<int> chunk_pos_data;
	std::vector<ChunkDrawParams> chunk_draw_params;

	BVH<Chunk*> chunks_BVH;

	World(long seed = 0);

	void setup();
	void generateMesh();
	void remeshChunk(Chunk* chunk);

	void addChunk(int x, int y, int z);
	Chunk* getChunk(const glm::vec3& pos);
	Chunk* getChunk(int x, int y, int z);
	bool peekChunk(const glm::vec3& pos, Chunk** chunk_out = nullptr);
	bool peekChunk(int x, int y, int z, Chunk** chunk_out = nullptr);

	void inspectPos(const glm::vec3& pos, BlockType** block_out, Chunk** chunk_out = nullptr);
	
	BlockType* inspectPos(glm::vec3 pos);
	bool inspectRay(const glm::vec3& pos, const glm::vec3& ray, BlockType** block_out, Chunk** chunk_out = nullptr);
	BlockType* inspectRay(const glm::vec3& pos, const glm::vec3& ray);

	void updateBlock(const glm::vec3& pos, BlockType new_type);
	void updateLookedAtBlock(Camera* camera, BlockType new_type);

	int encodeChunkPos(Chunk* chunk);
	Chunk::Group getChunkNeighbors(Chunk* chunk);

	static void drawWorld(Mesh* mesh, Camera* camera, void* obj);
};

#endif