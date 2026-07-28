#include "World.h"
#include "util.h"
#include "Debug.h"

int face_per_chunk;

World::World(long seed) : seed(seed) {
	face_per_chunk = 5 * CHUNK_AREA;
}

void World::setup() {
	std::srand(seed);
	int layers = 3;
	for(int y = 0; y < layers; y++) {
		chunks.add(0, y, 0);// chunks[Key(0, 0, 0)] = new Chunk(glm::ivec3(0, 0, 0));
		for (int r = 1; r <= chunk_radius; r++) {
			int x, z;
			x = z = r;
			for (z = r; z > -r; z--)
				chunks.add(x, y, z);
			z = -r;
			for (x = r; x > -r; x--)
				chunks.add(x, y, z);
			x = -r;
			for (z = -r; z < r; z++)
				chunks.add(x, y, z);
			z = r;
			for (x = -r; x < r; x++)
				chunks.add(x, y, z);
		}
	}

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

void World::update(float dt, Camera *camera, Input *input) {
	//Chunk* current_chunk = nullptr;
	static bool single_mine = true;
	if (input->keyPressed('Q'))
		single_mine = !single_mine;

	if (input->keyPressed('R')) {
		camera->transform.pos = glm::vec3(0, 6, 5);
		std::cout << "Camera Position reset!" << std::endl;
	}

	ChunkManager::RaycastResult cast = chunks.raycast(camera->transform.pos, camera->getLookDirection());
	if(cast.hit) {
		if ((input->mouse.left.held && !single_mine) || (input->mouse.left.pressed && single_mine)) {
			//std::cout << "MINING ";
			//printBlockData(cast.block, cast.chunk);
			updateBlock(cast.block, cast.chunk, BlockType::AIR);
		}
		if ((input->mouse.right.held && !single_mine) || (input->mouse.right.pressed && single_mine)) {
			//std::cout << "PLACING ";
			//printBlockData(cast.block, cast.chunk);
			placeBlock(cast, BlockType::DIRT);
		}
		if((input->mouse.middle.held && !single_mine) || (input->mouse.middle.pressed && single_mine)) {
			std::cout << "Middle Mouse Pressed!" << std::endl;
			blockBrushSphere(cast, 8, BlockType::DIRT);
		}
		if (input->keyPressed('E')) {
			putMeshWhereLooking(cast, "test_block");
			std::cout << "Inspect ";
			printBlockInfo(cast.block, cast.chunk);
		}
		if (input->keyPressed('I')) {
			traceBVHface(cast.chunk->faces_BVH);
		}
	}
}

void World::inspectPos(const glm::vec3 &pos, BlockType **block_out, Chunk **chunk_out) {
	glm::vec3 block_pos = pos / chunks.unit_length;

	int x_ch, y_ch, z_ch;
	x_ch = floor(block_pos.x / CHUNK_SIZE);
	y_ch = floor(block_pos.y / CHUNK_SIZE);
	z_ch = floor(block_pos.z / CHUNK_SIZE);
	Chunk *chunk = nullptr;
	if (!(chunk = chunks.get(x_ch, y_ch, z_ch)))
		return;

	int x_b = floor(block_pos.x -= x_ch * CHUNK_SIZE);
	int y_b = floor(block_pos.y -= y_ch * CHUNK_SIZE);
	int z_b = floor(block_pos.z -= z_ch * CHUNK_SIZE);

	if(block_out)
		*block_out = &chunk->blocks(x_b, y_b, z_b);
	if (chunk_out)
		*chunk_out = chunk;
}

BlockType* World::inspectPos(const glm::vec3 &pos) {
	BlockType *block;
	inspectPos(pos, &block);
	return block;
}

void World::updateBlock(BlockType *block, Chunk *chunk, BlockType new_type) {
	if (!block)
		return;
	*block = new_type;
	remeshChunk(chunk);

	int face = 0;
	std::cout << "updating block " << std::endl;
	printBlockInfo(block, chunk);

	if (chunk->blocks.onBoundary(block, &face)) {
		std::cout << "remeshing neighboring chunk" << std::endl;
		remeshChunk(chunks.getNeighbor(chunk, face));
	}
}

void World::updateBlocks(std::vector<BlockType*> &blocks, Chunk *chunk, BlockType new_type) {
	std::vector<Chunk*> modified_neighbors;
	for(BlockType *block : blocks) {
		*block = new_type;
		int face;
		if(chunk->blocks.onBoundary(block, &face)){
			Chunk *new_neighbor = chunks.getNeighbor(chunk, face);
			for(Chunk *neighbor : modified_neighbors) {
				if(new_neighbor == neighbor) {
					new_neighbor = nullptr;
					break;
				}
			}
			if(new_neighbor)
				modified_neighbors.push_back(chunks.getNeighbor(chunk, face));
		}
	}
	remeshChunk(chunk);
	for(Chunk *neighbor : modified_neighbors) 
		remeshChunk(neighbor);
}

void World::blockBrushSphere(ChunkManager::RaycastResult cast, float radius, BlockType new_type) {
	glm::vec3 pos = cast.pos + glm::vec3(chunks.unit_length / 2);
	BlockType *block;
	Chunk *chunk;
	inspectPos(pos, &block, &chunk);
	if(block && chunk) {
		std::cout << "block and chunk valid" << std::endl;
		std::vector<BlockType*> modified_blocks;
		int b_x, b_y, b_z;
		if(chunk->blocks.getCoords(block, b_x, b_y, b_z)) {
			glm::vec3 center(b_x, b_y, b_z);
			for(int x = b_x - radius; x < b_x + radius; x++) {
				for(int y = b_y - radius; y < b_y + radius; y++) {
					for(int z = b_z - radius; z < b_z + radius; z++) {
						if(z < 0 || z >= 32 || y < 0 || y >= 32 || x < 0 || x >= 32) {
							// std::cout << "some dimension out of range: " << x << ", " << y << ", " << z << std::endl;
							continue;
						}
						if(glm::length(glm::vec3(x, y, z) - center) < radius)
							modified_blocks.push_back(&chunk->blocks(x, y, z));
					}
				}
			}
			updateBlocks(modified_blocks, chunk, new_type);
		}
	}
}

void World::placeBlock(ChunkManager::RaycastResult cast, BlockType new_type) {
	glm::vec3 pos = cast.pos + glm::vec3(chunks.unit_length / 2); // move to center of block to avoid floating point nonsense
	switch (cast.face) {
		case 0: pos.y += chunks.unit_length; break;
		case 1: pos.y -= chunks.unit_length; break;
		case 2: pos.x += chunks.unit_length; break;
		case 3: pos.x -= chunks.unit_length; break;
		case 4: pos.z += chunks.unit_length; break;
		case 5: pos.z -= chunks.unit_length; break;
	}

	BlockType *block;
	Chunk *chunk;
	inspectPos(pos, &block, &chunk);
	if (block && chunk)
		updateBlock(block, chunk, new_type);
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

	glm::vec3 look_dir = camera->getLookDirection();
	float dot_criteria = cos(glm::radians(camera->aspect_ratio * camera->fov / 2));
	for (auto &chunk_obj : world->chunks.chunks) {
		Chunk *chunk = chunk_obj.second;

		glm::vec3 a, b, c, d;
		float length = CHUNK_SIZE * world->chunks.unit_length;
		a = chunk->getPosf();
		b = a + glm::vec3(0, 0, length);
		c = a + glm::vec3(length, 0, 0);
		d = a + glm::vec3(length, 0, length);
		if (
			glm::dot(look_dir, a - camera->transform.pos) < dot_criteria &&
			glm::dot(look_dir, b - camera->transform.pos) < dot_criteria &&
			glm::dot(look_dir, c - camera->transform.pos) < dot_criteria &&
			glm::dot(look_dir, d - camera->transform.pos) < dot_criteria) {
			world->chunks.draw_params[chunk->ID].instanceCount = 0;
		}
		else
			world->chunks.draw_params[chunk->ID].instanceCount = chunk->faces;
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