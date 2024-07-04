#pragma once
#ifndef WORLD_
#define WORLD_

#include "config.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "BVH.h"
#include "Camera.h"
#include "Input.h"

class World
{
public:
	int chunk_radius = 10;
	long seed;

	float ambient_lighting = 0.1;
	glm::vec3 sun_dir = glm::vec3(-1);

	ChunkManager chunks;

	World(long seed = 0);

	void setup();
	void generateMesh();
	void addChunkToMesh(Chunk* chunk);
	void remeshChunk(Chunk* chunk);

	void update(float dt, Camera* camera, Input* input);

	void inspectPos(const glm::vec3& pos, BlockType** block_out, Chunk** chunk_out = nullptr);
	
	BlockType* inspectPos(const glm::vec3& pos);
	bool inspectRay(const glm::vec3& pos, const glm::vec3& ray, BlockType** block_out, Chunk** chunk_out = nullptr);
	BlockType* inspectRay(const glm::vec3& pos, const glm::vec3& ray);

	void updateBlock(BlockType* block, Chunk* chunk, BlockType new_type);
	void placeBlock(ChunkManager::RaycastResult cast, BlockType new_type);
	void removeBlock(ChunkManager::RaycastResult cast, BlockType new_type);

	int encodeChunkPos(Chunk* chunk);

	static void drawWorld(Mesh* mesh, Camera* camera, void* obj);
};

#endif