#pragma once
#ifndef WORLD_
#define WORLD_

#include "config.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Chunk.h"

class World
{
public:
	float chunk_unit_dimension = 1.0;
	int max_chunk_radius = 4;

	std::vector<Chunk*> chunks;

	World() {}

	void setup();
	void generateMesh();
};

#endif