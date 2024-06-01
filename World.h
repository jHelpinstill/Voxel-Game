#pragma once
#ifndef WORLD_
#define WORLD_

#include "config.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Chunk.h"

class World
{
private:

public:
	float chunk_unit_dimension = 0.1;
	int chunk_radius = 5;

	std::unordered_map<ChunkKey, Chunk*> chunks;

	World() {}

	void setup();
	void generateMesh();

	void addChunk(int x, int y, int z);
	Chunk* getChunk(int x, int y, int z);

	void inspectPos(glm::vec3 pos, BlockType** block_at, Chunk** chunk_at = nullptr);
	void updateBlock(glm::vec3 pos, BlockType new_type);

};

#endif