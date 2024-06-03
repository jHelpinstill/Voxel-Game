#include "World.h"

float World::chunk_unit_length = 0.2;
std::vector<int> World::chunk_pos_data;
std::vector<ChunkDrawParams> World::chunk_draw_params;

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

	createShader("world_shader", "shaders/worldVertex.txt", "shaders/worldFragment.txt");
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
	glm::vec3 block_pos = pos / chunk_unit_length;
	int x, y, z;
	x = (int)(block_pos.x / CHUNK_SIZE); if (block_pos.x < 0) x--;
	y = (int)(block_pos.y / CHUNK_SIZE); if (block_pos.y < 0) y--;
	z = (int)(block_pos.z / CHUNK_SIZE); if (block_pos.z < 0) z--;
	Chunk* chunk = getChunk(x, y, z);

	x = (int)(block_pos.x - x * CHUNK_SIZE);
	y = (int)(block_pos.y - y * CHUNK_SIZE);
	z = (int)(block_pos.z - z * CHUNK_SIZE);

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

	removeMesh("world_mesh");
	Mesh* mesh = new Mesh(getTextureByName("chunk_texture"));
	meshes["world_mesh"] = mesh;

	mesh->verts.push_back(glm::vec3(0, 0, 0));
	mesh->verts.push_back(glm::vec3(0, 0, 1));
	mesh->verts.push_back(glm::vec3(1, 0, 0));
	mesh->verts.push_back(glm::vec3(1, 0, 1));

	mesh->uv_coords.push_back(glm::vec2(0, 0));
	mesh->uv_coords.push_back(glm::vec2(0.5, 0));
	mesh->uv_coords.push_back(glm::vec2(0, 1));
	mesh->uv_coords.push_back(glm::vec2(0.5, 1));

	int c_counter = 0;
	for (auto& chunk : chunks)
	{
		int num_instances = chunk.second->generateFaceData(mesh->instance_data);
		chunk_draw_params.push_back(ChunkDrawParams(4, num_instances, 0, mesh->instance_data.size() - num_instances));
		chunk_pos_data.push_back(encodeChunkPos(chunk.second));
	}
	
	mesh->shader = getShaderByName("world_shader");
	mesh->generateInstancedVAO();
	mesh->drawFunction = drawWorld;


	glGenBuffers(1, &mesh->SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mesh->SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, chunk_pos_data.size() * sizeof(int), chunk_pos_data.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mesh->SSBO);

	std::cout << "Finished world generation. World contains: " << chunks.size() << " chunks with " << mesh->instance_data.size() * 2 << " triangles" << std::endl;
}

#include <bitset>

int World::encodeChunkPos(Chunk* chunk)
{
	int x = chunk->x;
	int y = chunk->y;
	int z = chunk->z;

	int data = ((x + 512) & 1023);
	data |= ((y + 512) & 1023) << 10;
	data |= ((z + 512) & 1023) << 20;

	//std::bitset<32> bits(data);
	//std::cout << "chunk data: " << bits <<  ", " << data << " (" << x << ", " << y << ", " << z << ")" << std::endl;
	return data;
}

void World::drawWorld(Mesh* mesh, Camera* camera)
{
	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat() * mesh->transform.getMat());

	mesh->shader->setFloat("unit_length", chunk_unit_length);
	mesh->shader->setInt("chunk_size", CHUNK_SIZE);

	switch (mesh->style)
	{
	case Shader::VAOStyle::TEXTURED:
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		break;

	case Shader::VAOStyle::SOLID_COLORED:
		mesh->shader->setVec3("color", mesh->color);
		break;
	}

	//std::cout << "World::drawWorld()" << std::endl;

	glBindVertexArray(mesh->VAO);
	glMultiDrawArraysIndirect(GL_TRIANGLE_STRIP, chunk_draw_params.data(), chunk_draw_params.size(), sizeof(ChunkDrawParams));
}