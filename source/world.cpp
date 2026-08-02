#include "World.h"
#include "util.h"
#include "Debug.h"
#include <algorithm>

int face_per_chunk;

World::World(long seed) : seed(seed) {
	face_per_chunk = 5 * CHUNK_AREA;
}

void World::setup() {
	std::srand(seed);
	int layers = 3;
	for(int y = 0; y < layers; y++) {
		chunks.add(0, y, 0, false);
		for (int r = 1; r <= chunk_radius; r++) {
			int x, z;
			x = z = r;
			for (z = r; z > -r; z--)
				chunks.add(x, y, z, false);
			z = -r;
			for (x = r; x > -r; x--)
				chunks.add(x, y, z, false);
			x = -r;
			for (z = -r; z < r; z++)
				chunks.add(x, y, z, false);
			z = r;
			for (x = -r; x < r; x++)
				chunks.add(x, y, z, false);
		}
	}
	chunks.bvh.rebuild();

	traceBVHchunk(chunks.bvh);
	std::cout << "created " << chunks.size() << " chunks" << std::endl;

	std::vector<Shader::DefinePair> shader_defines = {
		{"COORD_X_BITPOS", std::to_string(chunks.shader_info.getCoordBitPos(0))},
		{"COORD_Y_BITPOS", std::to_string(chunks.shader_info.getCoordBitPos(1))},
		{"COORD_Z_BITPOS", std::to_string(chunks.shader_info.getCoordBitPos(2))},
		{"COORD_BITMASK", std::to_string(chunks.shader_info.getCoordBitMask())},
		
		{"COLOR_R_BITPOS", std::to_string(chunks.shader_info.getColorBitPos(0))},
		{"COLOR_G_BITPOS", std::to_string(chunks.shader_info.getColorBitPos(1))},
		{"COLOR_B_BITPOS", std::to_string(chunks.shader_info.getColorBitPos(2))},
		{"COLOR_BITMASK", std::to_string(chunks.shader_info.getColorBitMask())},
		{"COLOR_MAX", std::to_string((int)((1 << chunks.shader_info.color_bits) - 1))}
	};

	createShader("world_shader", ROOT + "shaders/worldVertex.txt", ROOT + "shaders/worldColorFragment.txt", shader_defines);
	generateMesh();
}

void World::update(float dt, CameraController *player, Input *input) {
	//Chunk* current_chunk = nullptr;
	static bool single_mine = true;
	if (input->keyPressed('Q'))
		single_mine = !single_mine;

	if (input->keyPressed('R')) {
		player->setPos(glm::vec3(0, 6, 5));
		player->velocity = glm::vec3(0);
		std::cout << "Camera Position reset!" << std::endl;
	}

	bool has_cast = false;
	ChunkManager::RaycastResult cast;
	if ((input->mouse.left.held && !single_mine) || (input->mouse.left.pressed && single_mine)) {
		if(!has_cast) {has_cast = true; cast = chunks.raycast(player->getPos(), player->getLookDirection());}
		if(cast.hit)
			chunks.setBlock(cast.pos, BlockType::AIR);
	}
	if ((input->mouse.right.held && !single_mine) || (input->mouse.right.pressed && single_mine)) {
		if(!has_cast) {has_cast = true; cast = chunks.raycast(player->getPos(), player->getLookDirection());}
		if(cast.hit)
			placeBlock(cast, BlockType::DIRT);
	}
	if((input->mouse.middle.held && !single_mine) || (input->mouse.middle.pressed && single_mine)) {
		std::cout << "Middle Mouse Pressed!" << std::endl;
		if(!has_cast) {has_cast = true; cast = chunks.raycast(player->getPos(), player->getLookDirection());}
		if(cast.hit)
			blockBrushSphere(cast, 8, block_types[block_type_select]);
	}
	if (input->keyPressed('E')) {
		if(!has_cast) {has_cast = true; cast = chunks.raycast(player->getPos(), player->getLookDirection());}
		if(cast.hit) {
			putMeshWhereLooking(cast, "test_block");
			std::cout << "Inspect ";
			printBlockInfo(cast.block, cast.chunk);
		}
	}
	if (input->keyPressed('I')) {
		if(!has_cast) {has_cast = true; cast = chunks.raycast(player->getPos(), player->getLookDirection());}
		if(cast.hit) {
			traceBVHface(cast.chunk->faces_BVH);
			drawChunkBoundaries(cast.chunk, glm::vec3(0, 0, 1));
		}
	}
	if(input->keyPressed('C')) {
		clearDebugGeometry();
	}
	if(input->mouse.scroll) {
		block_type_select += (int)input->mouse.scroll;
		int num_types = sizeof(block_types) / sizeof(block_types[0]);
		if(block_type_select >= num_types) block_type_select = 0;
		else if(block_type_select < 0) block_type_select = num_types - 1;
		switch(block_types[block_type_select]) {
			case BlockType::DIRT: 
				getDecalByName("dirt_select")->awake = true;
				getDecalByName("air_select")->awake = false;
				getDecalByName("stone_select")->awake = false;
				break;
			case BlockType::AIR: 
				getDecalByName("dirt_select")->awake = false;
				getDecalByName("air_select")->awake = true;
				getDecalByName("stone_select")->awake = false;
				break;
			case BlockType::STONE: 
				getDecalByName("dirt_select")->awake = false;
				getDecalByName("air_select")->awake = false;
				getDecalByName("stone_select")->awake = true;
				break;
		}
	}
	remeshModifiedChunks();
}

void World::blockBrushSphere(ChunkManager::RaycastResult cast, float radius, BlockType new_type) {
	glm::vec3 center = cast.pos;
	float block_radius = radius * chunks.unit_length;

	for(int x = - radius; x < radius; x++) {
		for(int y = - radius; y < radius; y++) {
			for(int z = - radius; z < radius; z++) {
				glm::vec3 pos(x * chunks.unit_length, y * chunks.unit_length, z * chunks.unit_length);
				if(glm::length(pos) < block_radius)
					chunks.setBlock(pos + center, new_type);
			}
		}
	}
}

void World::placeBlock(ChunkManager::RaycastResult cast, BlockType new_type) {
	glm::vec3 pos = cast.pos;
	switch (cast.face) {
		case 0: pos.y += chunks.unit_length; break;
		case 1: pos.y -= chunks.unit_length; break;
		case 2: pos.x += chunks.unit_length; break;
		case 3: pos.x -= chunks.unit_length; break;
		case 4: pos.z += chunks.unit_length; break;
		case 5: pos.z -= chunks.unit_length; break;
	}
	chunks.setBlock(pos, new_type);
}

void World::generateMesh() {
	std::cout << "Generating World mesh..." << std::endl;

	removeMesh("world_mesh");
	Mesh *world_mesh = new Mesh(getTextureByName("chunk_texture"));
	meshes["world_mesh"] = world_mesh;

	world_mesh->verts.push_back(glm::vec3(0, 0, 0));
	world_mesh->verts.push_back(glm::vec3(0, 0, 1));
	world_mesh->verts.push_back(glm::vec3(1, 0, 0));
	world_mesh->verts.push_back(glm::vec3(1, 0, 1));

	world_mesh->uv_coords.push_back(glm::vec2(0, 0));
	world_mesh->uv_coords.push_back(glm::vec2(0.5, 0));
	world_mesh->uv_coords.push_back(glm::vec2(0, 1));
	world_mesh->uv_coords.push_back(glm::vec2(0.5, 1));

	chunks.draw_params.clear();
	chunks.pos_data.clear();

	int chunk_counter = 0;
	int face_counter = 0;
	for (auto &bucket : chunks.chunks) {
		Chunk *chunk = bucket.second;

		int num_instances = chunk->generateFaceData(world_mesh->instance_data, chunks.getNeighbors(chunk));
		int padding = face_per_chunk - num_instances;
		for (int i = 0; i < padding; i++)
			world_mesh->instance_data.push_back(0);
		chunks.draw_params.push_back(ChunkManager::DrawParams(4, num_instances, 0, world_mesh->instance_data.size() - face_per_chunk));
		chunks.pos_data.push_back(encodeChunkPos(chunk));
		chunk->ID = chunk_counter++;

		face_counter += num_instances;
	}
	
	world_mesh->shader = getShaderByName("world_shader");
	world_mesh->vao->makeInstanced(world_mesh->verts, world_mesh->instance_data);
	world_mesh->drawFunc = drawWorld;
	world_mesh->parent_obj = this;

	glGenBuffers(1, &chunks.pos_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunks.pos_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, chunks.pos_data.size() * sizeof(int), chunks.pos_data.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, chunks.pos_SSBO);

	std::cout << "Finished world generation. World contains: " << chunks.size() << " chunks with " << face_counter << " faces" << std::endl;
}

void World::remeshModifiedChunks() {
	for(auto &chunk_pair : chunks.chunks) {
		Chunk *chunk = chunk_pair.second;
		if(chunk->modified)
			remeshChunk(chunk);
	}
}

void World::remeshChunk(Chunk *chunk) {
	if (!chunk)
		return;
	std::cout << "remeshing chunk at: " << chunk->x << ", " << chunk->y << ", " << chunk->z << std::endl;

	ChunkManager::DrawParams *chunk_params = &chunks.draw_params[chunk->ID];
	std::vector<int> chunk_instance_data;

	int chunk_num_instances = chunk->generateFaceData(chunk_instance_data, chunks.getNeighbors(chunk));
	int padding = face_per_chunk - chunk_num_instances;
	for (int i = 0; i < padding; i++)
		chunk_instance_data.push_back(0);
	chunk_params->instanceCount = chunk_num_instances;

	Mesh *mesh = getMeshByName("world_mesh");

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vao->data_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, chunk_params->baseInstance * sizeof(int), chunk_num_instances * sizeof(int), chunk_instance_data.data());
}

void World::addChunkToMesh(Chunk *chunk) {
	Mesh *world_mesh = getMeshByName("world_mesh");

	int num_instances = chunk->generateFaceData(world_mesh->instance_data, chunks.getNeighbors(chunk));
	// The padding seems to be necessary to give each chunk a uniform portion of the VBO, so they can
	// be re-meshed larger or smaller without overwriting the other chunks
	int padding = face_per_chunk - num_instances;
	for (int i = 0; i < padding; i++)
		world_mesh->instance_data.push_back(0);
	// 4 what, bytes? TODO: what is count in DrawParams referring to?
	chunks.draw_params.push_back(ChunkManager::DrawParams(
		4, 				// something (bytes?)
		num_instances, 	// how many instances in this section of the VBO (this chunk)
		0, 				// first, I guess in the whole VBO? (not just this chunk?)
		world_mesh->instance_data.size() - face_per_chunk	// Position of the start of this chunk's instances
	));
	chunks.pos_data.push_back(encodeChunkPos(chunk));

	chunk->ID = chunks.size();

	world_mesh->vao->reset();
	world_mesh->vao->makeInstanced(world_mesh->verts, world_mesh->instance_data);

	glDeleteBuffers(1, &chunks.pos_SSBO);
	glGenBuffers(1, &chunks.pos_SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunks.pos_SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, chunks.pos_data.size() * sizeof(int), chunks.pos_data.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, chunks.pos_SSBO);
}

int World::encodeChunkPos(Chunk *chunk) {
	const int chunk_pos_mask = (1 << chunks.chunk_pos_bits) - 1;
	const int half_chunk_mask = (chunk_pos_mask / 2) + 1;

	int x = chunk->x;
	int y = chunk->y;
	int z = chunk->z;

	int data = 0; int offset = 0;
	data |= ((x + half_chunk_mask) & chunk_pos_mask) << offset; offset += chunks.chunk_pos_bits;
	data |= ((y + half_chunk_mask) & chunk_pos_mask) << offset; offset += chunks.chunk_pos_bits;
	data |= ((z + half_chunk_mask) & chunk_pos_mask) << offset;

	return data;
}

void World::drawWorld(Mesh *mesh, Camera *camera) {
	World *world = (World*)mesh->parent_obj;

	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat() * mesh->transform.getMat() * glm::scale(glm::mat4(1.0), glm::vec3(world->chunks.unit_length)));
	mesh->shader->setInt("chunk_size", CHUNK_SIZE);
	mesh->shader->setFloat("ambient", world->ambient_lighting);

	glm::vec3 lighting_dir = world->sun_dir;
	float t = glm::dot(glm::normalize(lighting_dir), glm::vec3(0, 1, 0));
	if (t <= 0 && t > -0.2)
		lighting_dir *= t * -5;
	else if (t > 0)
		lighting_dir = glm::vec3(0);

	mesh->shader->setVec3("light_dir", lighting_dir);

	switch (mesh->vao->style) {
		case VAO::Style::TEXTURED:
			glBindTexture(GL_TEXTURE_2D, mesh->texture);
			break;

		case VAO::Style::SOLID_COLORED:
			mesh->shader->setVec3("color", mesh->color);
			break;
	}

	glBindVertexArray(mesh->vao->ID);

	unsigned int indirect_command_buffer;
	glGenBuffers(1, &indirect_command_buffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_command_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, (unsigned int)(world->chunks.draw_params.size() * sizeof(ChunkManager::DrawParams)), world->chunks.draw_params.data(), GL_DYNAMIC_DRAW);

	glMultiDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr, world->chunks.draw_params.size(), sizeof(ChunkManager::DrawParams));

	glBindVertexArray(0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glDeleteBuffers(1, &indirect_command_buffer);
}