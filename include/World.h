#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "config.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "BVH.h"
#include "CameraController.h"
#include "Input.h"

class World {
public:
	int chunk_radius = 10;
	long seed;

	float ambient_lighting = 0.5;
	glm::vec3 sun_dir = glm::vec3(-1);

	ChunkManager chunks;
	BlockType block_types[3] = {
		BlockType::DIRT, BlockType::STONE, BlockType::AIR
	};
	int block_type_select = 0;

	World(long seed = 0);

	void setup();
	void generateMesh();
	void addChunkToMesh(Chunk *chunk);
	void remeshChunk(Chunk *chunk);
	void remeshModifiedChunks();

	void update(float dt, CameraController *player, Input *input);

	void blockBrushSphere(ChunkManager::RaycastResult cast, float radius, BlockType new_type);
	void placeBlock(ChunkManager::RaycastResult cast, BlockType new_type);

	int encodeChunkPos(Chunk *chunk);

	static void drawWorld(Mesh *mesh, Camera *camera);
};

#endif