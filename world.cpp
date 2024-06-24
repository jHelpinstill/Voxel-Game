#include "World.h"
#include "util.h"

int face_per_chunk;

World::World(long seed) : seed(seed)
{
	face_per_chunk = 4 * CHUNK_SIZE * CHUNK_SIZE;
}

void World::setup()
{
	std::srand(seed);
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

	createShader("world_shader", "shaders/worldVertex.txt", "shaders/worldColorFragment.txt");
	generateMesh();
}

void World::addChunk(int x, int y, int z)
{
	Chunk::Key key(x, y, z);
	if (chunks.find(key) != chunks.end())
	{
		delete chunks[key];
		chunks.erase(key);
	}
	chunks[key] = new Chunk(x, y, z, std::rand(), chunk_unit_length);
}

Chunk* World::getChunk(int x, int y, int z)
{
	Chunk::Key key(x, y, z);
	if (chunks.find(key) == chunks.end())
		chunks[key] = new Chunk(x, y, z, std::rand());
	return chunks[key];
}

bool World::peekChunk(int x, int y, int z, Chunk** chunk_at)
{
	Chunk::Key key(x, y, z);
	bool exists = false;
	if (chunks.find(key) != chunks.end())
		exists = true;
	if (exists && chunk_at)
		*chunk_at = chunks[key];
	return exists;
}

void World::inspectPos(glm::vec3 pos, BlockType** block_at, Chunk** chunk_at)
{
	Mesh* world_mesh = getMeshByName("world_mesh");
	//pos -= world_mesh->transform.pos;

	glm::vec3 block_pos = pos / chunk_unit_length;
	int x_ch, y_ch, z_ch;
	x_ch = (int)(block_pos.x / CHUNK_SIZE); if (block_pos.x < 0) x_ch--;
	y_ch = (int)(block_pos.y / CHUNK_SIZE); if (block_pos.y < 0) y_ch--;
	z_ch = (int)(block_pos.z / CHUNK_SIZE); if (block_pos.z < 0) z_ch--;
	Chunk* chunk = getChunk(x_ch, y_ch, z_ch);

	int x_b = (int)(block_pos.x -= x_ch * CHUNK_SIZE);
	int y_b = (int)(block_pos.y -= y_ch * CHUNK_SIZE);
	int z_b = (int)(block_pos.z -= z_ch * CHUNK_SIZE);

	std::cout << "inspecting block at " << pos.x << ", " << pos.y << ", " << pos.z << ", block index " << x_b << ", " << y_b << ", " << z_b << std::endl;
	if(block_at)
		*block_at = &chunk->blocks[x_b][y_b][z_b];
	if (chunk_at)
		*chunk_at = chunk;
}

BlockType* World::inspectPos(glm::vec3 pos)
{
	BlockType* block;
	inspectPos(pos, &block);
	return block;
}

bool World::inspectRay(glm::vec3 pos, glm::vec3 dir, BlockType** block_at, Chunk** chunk_at)
{
	//std::cout << "Camera Pos: " << vec2string(pos) << std::endl;
	//std::cout << "look direction: " << vec2string(dir) << std::endl;

	Mesh* world_mesh = getMeshByName("world_mesh");
	//pos -= world_mesh->transform.pos;

	dir = glm::normalize(dir);

	glm::vec3 block_pos = pos / chunk_unit_length;
	int chunk_pos[3];
	chunk_pos[0] = floor(block_pos.x / CHUNK_SIZE);
	chunk_pos[1] = floor(block_pos.y / CHUNK_SIZE);
	chunk_pos[2] = floor(block_pos.z / CHUNK_SIZE);
	Chunk* chunk;
	if (!peekChunk(chunk_pos[0], chunk_pos[1], chunk_pos[2], &chunk))
		return false;
	
	int x_b = (int)(block_pos.x -= chunk_pos[0] * CHUNK_SIZE);
	int y_b = (int)(block_pos.y -= chunk_pos[1] * CHUNK_SIZE);
	int z_b = (int)(block_pos.z -= chunk_pos[2] * CHUNK_SIZE);
	
	glm::vec3 accumulator(0);
	accumulator.x = block_pos.x - floor(block_pos.x);
	accumulator.y = block_pos.y - floor(block_pos.y);
	accumulator.z = block_pos.z - floor(block_pos.z);
	while (chunk->blocks[x_b][y_b][z_b] == BlockType::AIR)// || chunk->blocks[x_b][y_b][z_b] == BlockType::STONE)
	{
		//chunk->blocks[x_b][y_b][z_b] = BlockType::STONE;
		
		//bool dom_step = true;
		bool take_step = true;
		int rank = 0;
		for (int i = 0; i < 3; i++)
		{
			if (accumulator[i] > 1 || accumulator[i] < 0)
			{
				accumulator[i] += (dir[i] < 0) ? 1 : -1;
				block_pos[i] += (dir[i] < 0) ? -1 : 1;
				//dom_step = false;
				take_step = false;
				//std::cout << "stepping toward " << i << std::endl;
				break;
			}
		}
		if (take_step)
			accumulator += dir;

		bool next_chunk = false;
		for (int i = 0; i < 3; i++)
		{
			if (block_pos[i] < 0)
			{
				block_pos[i] = CHUNK_SIZE + block_pos[i];
				chunk_pos[i]--;
				next_chunk = true;
			}
			else if (block_pos[i] >= CHUNK_SIZE)
			{
				block_pos[i] = CHUNK_SIZE - block_pos[i];
				chunk_pos[i]++;
				next_chunk = true;
			}
		}
		if (next_chunk)
			if (!peekChunk(chunk_pos[0], chunk_pos[1], chunk_pos[2], &chunk))
				return false;

		x_b = (int)(block_pos.x);
		y_b = (int)(block_pos.y);
		z_b = (int)(block_pos.z);

		//std::cout << "block_pos: " << vec2string(block_pos) << std::endl;
		//std::cout << "accumulator: " << vec2string(accumulator) << std::endl;
		//std::cout << "b coords: " << x_b << ", " << y_b << ", " << z_b << "\n" << std::endl;

	}

	if (block_at)
		*block_at = &chunk->blocks[x_b][y_b][z_b];
	if (chunk_at)
		*chunk_at = chunk;
}

BlockType* World::inspectRay(glm::vec3 pos, glm::vec3 dir)
{
	BlockType* block;
	if (!inspectRay(pos, dir, &block))
		return nullptr;
	return block;
}

void World::updateBlock(glm::vec3 pos, BlockType new_type)
{
	BlockType* block;
	Chunk* chunk;
	inspectPos(pos, &block, &chunk);

	*block = new_type;
	remeshChunk(chunk);
}

void World::updateLookedAtBlock(Camera* camera, BlockType new_type)
{
	BlockType* block;
	Chunk* chunk;
	if (!inspectRay(camera->transform.pos, camera->getLookDirection(), &block, &chunk))
		return;
	*block = new_type;
	remeshChunk(chunk);
}

void World::remeshChunk(Chunk* chunk)
{
	ChunkDrawParams* chunk_params = &chunk_draw_params[chunk->ID];

	std::vector<int> instance_data;

	int num_instances = chunk->generateFaceData(instance_data);
	int padding = face_per_chunk - num_instances;
	for (int i = 0; i < padding; i++)
		instance_data.push_back(0);
	chunk_params->instanceCount = num_instances;

	Mesh* mesh = getMeshByName("world_mesh");

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vao->data_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, chunk_params->baseInstance * sizeof(int), num_instances * sizeof(int), instance_data.data());
	//int i = chunk_params->baseInstance;
	//for (int instance : instance_data)
	//{
	//	mesh->instance_data[i++] = instance;
	//}

	std::cout << "Remeshed chunk " << chunk->ID << ", now has " << num_instances << " faces" << std::endl;
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

	chunk_draw_params.clear();
	chunk_pos_data.clear();

	int chunk_counter = 0;
	for (auto& bucket : chunks)
	{
		Chunk* chunk = bucket.second;

		int num_instances = chunk->generateFaceData(mesh->instance_data);
		int padding = face_per_chunk - num_instances;
		for (int i = 0; i < padding; i++)
			mesh->instance_data.push_back(0);
		chunk_draw_params.push_back(ChunkDrawParams(4, num_instances, 0, mesh->instance_data.size() - face_per_chunk));
		chunk_pos_data.push_back(encodeChunkPos(chunk));
		chunk->ID = chunk_counter++;
	}
	
	mesh->shader = getShaderByName("world_shader");
	mesh->vao->makeInstanced(mesh->verts, mesh->instance_data);
	mesh->drawFunction = drawWorld;
	//mesh->transform.translate(glm::vec3(0, -3, 0));
	mesh->parent_obj = this;


	glGenBuffers(1, &chunk_pos_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunk_pos_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, chunk_pos_data.size() * sizeof(int), chunk_pos_data.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, chunk_pos_SSBO);

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

void World::drawWorld(Mesh* mesh, Camera* camera, void* obj)
{
	World* world = (World*)obj;

	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat() * mesh->transform.getMat() * glm::scale(glm::mat4(1.0), glm::vec3(world->chunk_unit_length)));
	//mesh->shader->setFloat("unit_length", world->chunk_unit_length);
	mesh->shader->setInt("chunk_size", CHUNK_SIZE);
	mesh->shader->setFloat("ambient", world->ambient_lighting);

	glm::vec3 lighting_dir = world->sun_dir;
	float t = glm::dot(glm::normalize(lighting_dir), glm::vec3(0, 1, 0));
	if (t <= 0 && t > -0.2)
		lighting_dir *= t * -5;
	else if (t > 0)
		lighting_dir = glm::vec3(0);

	mesh->shader->setVec3("light_dir", lighting_dir);

	switch (mesh->style)
	{
	case Shader::VAOStyle::TEXTURED:
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		break;

	case Shader::VAOStyle::SOLID_COLORED:
		mesh->shader->setVec3("color", mesh->color);
		break;
	}

	glm::vec3 look_dir = camera->getLookDirection();
	float dot_criteria = cos(glm::radians(camera->aspect_ratio * camera->fov / 2));
	for (auto& chunk_obj : world->chunks)
	{
		Chunk* chunk = chunk_obj.second;
		glm::vec3 a, b, c, d;
		float length = CHUNK_SIZE * world->chunk_unit_length;
		a = chunk->getPosf();
		b = a + glm::vec3(0, 0, length);
		c = a + glm::vec3(length, 0, 0);
		d = a + glm::vec3(length, 0, length);
		if (
			glm::dot(look_dir, a - camera->transform.pos) < dot_criteria &&
			glm::dot(look_dir, b - camera->transform.pos) < dot_criteria &&
			glm::dot(look_dir, c - camera->transform.pos) < dot_criteria &&
			glm::dot(look_dir, d - camera->transform.pos) < dot_criteria)
		{
			world->chunk_draw_params[chunk->ID].instanceCount = 0;
		}
		else
			world->chunk_draw_params[chunk->ID].instanceCount = chunk->faces;
	}

	glBindVertexArray(mesh->vao->ID);

	unsigned int indirect_command_buffer;
	glGenBuffers(1, &indirect_command_buffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_command_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, (unsigned int)(world->chunk_draw_params.size() * sizeof(ChunkDrawParams)), world->chunk_draw_params.data(), GL_DYNAMIC_DRAW);

	glMultiDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr, world->chunk_draw_params.size(), sizeof(ChunkDrawParams));

	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glDeleteBuffers(1, &indirect_command_buffer);
}