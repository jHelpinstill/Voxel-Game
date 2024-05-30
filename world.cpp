#include "World.h"

void World::setup()
{
	chunks.push_back(new Chunk(glm::ivec3(0, 0, 0)));
	for (int r = 1; r < max_chunk_radius; r++)
	{
		int x, z;

		x = z = r;
		for (x; x <= r; x++)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
		for (z; z >= -r; z--)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
		for (x; x >= -r; x--)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
		for (z; z < r; z++)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
	}
}

void generateMesh()
{
	//std::vector<float> verts
	// top
}