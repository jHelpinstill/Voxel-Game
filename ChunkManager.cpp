#include "ChunkManager.h"

void ChunkManager::add(int x, int y, int z)
{
	Chunk::Key key(x, y, z);
	if (chunks.find(key) != chunks.end())
	{
		delete chunks[key];
		chunks.erase(key);
	}
	chunks[key] = new Chunk(x, y, z, std::rand(), unit_length);
}

Chunk* ChunkManager::get(const glm::vec3& pos)
{
	glm::vec3 block_pos = pos / unit_length;
	int x, y, z;
	x = (int)(block_pos.x / CHUNK_SIZE); if (block_pos.x < 0) x--;
	y = (int)(block_pos.y / CHUNK_SIZE); if (block_pos.y < 0) y--;
	z = (int)(block_pos.z / CHUNK_SIZE); if (block_pos.z < 0) z--;

	return get(x, y, z);
}

Chunk* ChunkManager::get(int x, int y, int z)
{
	Chunk::Key key(x, y, z);
	if (chunks.find(key) != chunks.end())
		return chunks[key];

	return nullptr;
}

Chunk::Group ChunkManager::getNeighbors(Chunk* chunk)
{
	Chunk::Group neighbors(6);
	int x = chunk->x; int y = chunk->y; int z = chunk->z;

	neighbors[0] = get(x, y - 1, z);
	neighbors[1] = get(x, y + 1, z);
	neighbors[2] = get(x - 1, y, z);
	neighbors[3] = get(x + 1, y, z);
	neighbors[4] = get(x, y, z - 1);
	neighbors[5] = get(x, y, z + 1);

	return neighbors;
}

int ChunkManager::size()
{
	return chunks.size();
}