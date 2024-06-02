#include "World.h"

World::World()
{
	Chunk::unit_length = chunk_unit_length;
}

void World::setup()
{
	addChunk(0, 0, 0);// chunks[Key(0, 0, 0)] = new Chunk(glm::ivec3(0, 0, 0));
	for (int r = 1; r <= chunk_radius; r++)
	{
		int x, z;
		x = z = r;
		for (z = r; z > -r; z--)
			addChunk(x, 0, z);// chunks[Key(x, 0, z)] = new Chunk(glm::ivec3(x, 0, z));
		z = -r;
		for (x = r; x > -r; x--)
			addChunk(x, 0, z);//chunks[Key(x, 0, z)] = new Chunk(glm::ivec3(x, 0, z));
		x = -r;
		for (z = -r; z < r; z++)
			addChunk(x, 0, z);//chunks[Key(x, 0, z)] = new Chunk(glm::ivec3(x, 0, z));
		z = r;
		for (x = -r; x < r; x++)
			addChunk(x, 0, z);//chunks[Key(x, 0, z)] = new Chunk(glm::ivec3(x, 0, z));
	}

	std::cout << "created " << chunks.size() << " chunks" << std::endl;

	generateMesh();
}

void World::addChunk(int x, int y, int z)
{
	ChunkKey key(x, y, z);
	if (chunks.find(key) != chunks.end())
	{
		delete chunks[key];
		chunks.erase(key);
	}
	chunks[key] = new Chunk(x, y, z);
}

Chunk* World::getChunk(int x, int y, int z)
{
	ChunkKey key(x, y, z);
	if (chunks.find(key) == chunks.end())
		chunks[key] = new Chunk(x, y, z);
	return chunks[key];
}

void World::inspectPos(glm::vec3 pos, BlockType** block_at, Chunk** chunk_at)
{
	//Mesh* world_mesh = getMeshByName("world_mesh");
	//pos -= world_mesh->transform.pos;
	pos /= chunk_unit_length;
	int x, y, z;
	x = (int)(pos.x / CHUNK_SIZE); if (pos.x < 0) x--;
	y = (int)(pos.y / CHUNK_SIZE); if (pos.y < 0) y--;
	z = (int)(pos.z / CHUNK_SIZE); if (pos.z < 0) z--;
	Chunk* chunk = getChunk(x, y, z);

	x = (int)(pos.x - x * CHUNK_SIZE);
	y = (int)(pos.y - y * CHUNK_SIZE);
	z = (int)(pos.z - z * CHUNK_SIZE);

	std::cout << "inspecting block at " << pos.x << ", " << pos.y << ", " << pos.z << ", block index " << x << ", " << y << ", " << z << std::endl;
	*block_at = &chunk->blocks[x][y][z];
	if (chunk_at)
		*chunk_at = chunk;
}
void World::updateBlock(glm::vec3 pos, BlockType new_type)
{
	BlockType* block;
	Chunk* chunk;
	inspectPos(pos, &block, &chunk);

	*block = new_type;

	chunk->generateMesh();
}

void World::generateMesh()
{
	std::cout << "Generating World mesh..." << std::endl;

	//std::vector<glm::vec3> verts;
	//std::vector<glm::vec2> uv_coords;

	// top
	for (auto& chunk : chunks)
	{
		chunk.second->generateMesh();
	}

	//Mesh* world_mesh = new Mesh("world_mesh", verts, getTextureByName("dirt_block"), uv_coords);
	//world_mesh->attachShader(getShaderByName("texture_shader"));
	//world_mesh->transform.translate(glm::vec3(0, -16 * chunk_unit_dimension, 0));
	//removeMesh("world_mesh");
	//meshes["world_mesh"] = world_mesh;
	//
	//std::cout << "Finished world generation. World mesh contains " << verts.size() << " vertices." << std::endl;
}