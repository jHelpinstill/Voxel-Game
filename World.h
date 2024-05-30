#pragma once
#ifndef WORLD_
#define WORLD_

#include "config.h"
#include "Mesh.h"
#include "Chunk.h"

class World
{
public:

	int max_chunk_radius = 5;

	std::vector<Chunk*> chunks;

	World() {}

	void setup();

};

#endif