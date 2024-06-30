#include "ChunkManager.h"

bool ChunkManager::add(int x, int y, int z)
{
	Chunk::Key key(x, y, z);
	if (chunks.find(key) != chunks.end())
	{
		return false;
	}
	Chunk* chunk = new Chunk(x, y, z, std::rand(), unit_length);
	chunks[key] = chunk;
	bvh.root->addDataNode(glm::vec3(x, y, z) * (float)CHUNK_SIZE * unit_length, chunk);
	bvh.rebuild();
	return true;
}

Chunk* ChunkManager::get(const glm::vec3& pos)
{
	glm::vec3 block_pos = pos / unit_length;
	int x, y, z;
	x = floor(block_pos.x / CHUNK_SIZE);
	y = floor(block_pos.y / CHUNK_SIZE);
	z = floor(block_pos.z / CHUNK_SIZE);

	return get(x, y, z);
}

Chunk* ChunkManager::get(int x, int y, int z)
{
	Chunk::Key key(x, y, z);
	if (chunks.find(key) != chunks.end())
		return chunks[key];

	return nullptr;
}

Chunk* ChunkManager::getNeighbor(Chunk* chunk, int face)
{
	int x = chunk->x; int y = chunk->y; int z = chunk->z;
	switch (face)
	{
		case 0: return get(x, y + 1, z);
		case 1: return get(x, y - 1, z);
		case 2: return get(x + 1, y, z);
		case 3: return get(x - 1, y, z);
		case 4: return get(x, y, z + 1);
		case 5: return get(x, y, z - 1);
	}
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

ChunkManager::RaycastResult ChunkManager::raycast(const glm::vec3& pos, const glm::vec3& ray)
{
	BVH<Chunk*>::RaycastResult cast = bvh.raycast(pos, ray);
	RaycastResult result{};
	if (cast.hit)
	{
		result.hit = cast.hit;
		result.chunk = *cast.obj;

		Chunk::RaycastResult hit_block = result.chunk->last_successful_raycast;
		result.block = hit_block.obj->block;
		result.face = hit_block.obj->norm;
		result.pos = hit_block.pos;
	}
	return result;
}

bool ChunkManager::raycastChunk(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3& chunk_pos, Chunk** chunk)
{
	//Chunk::RaycastResult chunk_result = (*chunk)->raycast(pos, ray);
	return (*chunk)->raycast(pos, ray).hit;
}
void ChunkManager::expandToFitChunk(const glm::vec3& pos, Chunk** chunk, glm::vec3& min, glm::vec3& max)
{
	min = glm::min(min, pos);
	max = glm::max(max, pos + util::XYZ * (float)CHUNK_SIZE * (*chunk)->unit_length);
}