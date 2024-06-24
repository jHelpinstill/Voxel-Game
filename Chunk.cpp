#include "Chunk.h"
#include "util.h"

Chunk::Chunk(int x, int y, int z, long seed, float unit_length) : x(x), y(y), z(z), seed(seed), unit_length(unit_length)
{
	this->ID = -1;
	std::stringstream buffer;
	buffer << "chunkmesh" << x << y << z;
	buffer >> mesh_name;

	for (int xb = 0; xb < CHUNK_SIZE; xb++) for (int zb = 0; zb < CHUNK_SIZE; zb++) for (int yb = 0; yb < CHUNK_SIZE; yb++)
	{
		glm::vec3 pos = getPosf();
		pos.x += xb * unit_length;
		pos.y += yb * unit_length;
		pos.z += zb * unit_length;

		if (pos.y < 2.0)
		{
			blocks[xb][yb][zb] = BlockType::STONE;
			continue;
		}
		if (pos.y < 
			3.2 +
			2.1 * (sin(pos.x * 3.0 / 64) * cos(pos.z * 7.0 / 64)) +
			0.5 * (sin(pos.x * 25.0 / 64) * cos(pos.z * 29.0 / 64))
			)
			blocks[xb][yb][zb] = BlockType::DIRT;
		else
		{
			for(yb; yb < CHUNK_SIZE; yb++)
				blocks[xb][yb][zb] = BlockType::AIR;
		}
	}
}

glm::vec3 Chunk::getPosf()
{
	return glm::vec3(x, y, z) * (float)CHUNK_SIZE * unit_length;
}

int Chunk::generateFaceData(std::vector<int>& data)
{
	faces_BVH.reset();

	int instances = 0;
	std::srand(seed);
	glm::vec3 chunk_pos = getPosf() * (1.0f / unit_length);
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = CHUNK_SIZE - 1; y >= 0; y--)
			{
				int rand_num = std::rand();
				if (blocks[x][y][z] != BlockType::AIR)
					continue;

				glm::vec3 pos = chunk_pos + glm::vec3(x, y, z);
				//top
				if (y != 0) if (blocks[x][y - 1][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 0, getBlockColor(blocks[x][y - 1][z], 0, rand_num)));
					instances++;
					faces_BVH.root->addDataNode(pos - util::Y, 0);
				}

				//bottom
				if (y != CHUNK_SIZE - 1) if (blocks[x][y + 1][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 1, getBlockColor(blocks[x][y + 1][z], 1, rand_num)));
					instances++;
					faces_BVH.root->addDataNode(pos + util::Y, 1);
				}

				//left
				if (x != 0) if (blocks[x - 1][y][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 2, getBlockColor(blocks[x - 1][y][z], 2, rand_num)));
					instances++;
					faces_BVH.root->addDataNode(pos - util::X, 2);
				}

				//right
				if (x != CHUNK_SIZE - 1) if (blocks[x + 1][y][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 3, getBlockColor(blocks[x + 1][y][z], 3, rand_num)));
					instances++;
					faces_BVH.root->addDataNode(pos + util::X, 3);
				}

				//forward
				if (z != 0) if (blocks[x][y][z - 1] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 4, getBlockColor(blocks[x][y][z - 1], 4, rand_num)));
					instances++;
					faces_BVH.root->addDataNode(pos - util::Z, 4);
				}

				//back
				if (z != CHUNK_SIZE - 1) if (blocks[x][y][z + 1] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 5, getBlockColor(blocks[x][y][z + 1], 5, rand_num)));
					instances++;
					faces_BVH.root->addDataNode(pos + util::Z, 5);
				}
			}
		}
	}

	faces_BVH.root->split();
	faces = instances;
	return instances;
}

//Mesh* Chunk::generateMesh()
//{
//	removeMesh(mesh_name);
//	mesh = new Mesh(getTextureByName("chunk_texture"));
//	meshes[mesh_name] = mesh;
//
//	mesh->verts.push_back(glm::vec3(0, 0, 0));
//	mesh->verts.push_back(glm::vec3(0, 0, 1));
//	mesh->verts.push_back(glm::vec3(1, 0, 0));
//	mesh->verts.push_back(glm::vec3(1, 0, 1));
//
//	mesh->uv_coords.push_back(glm::vec2(0, 0));
//	mesh->uv_coords.push_back(glm::vec2(0.5, 0));
//	mesh->uv_coords.push_back(glm::vec2(0, 1));
//	mesh->uv_coords.push_back(glm::vec2(0.5, 1));
//
//	generateFaceData(mesh->instance_data);
//
//	mesh->shader = getShaderByName("chunk_shader");
//	mesh->generateInstancedVAO();
//	mesh->drawFunction = drawInstanced;
//	mesh->transform.translate(getPosf());
//	//std::cout << mesh_name << " has " << mesh->verts.size() << " verts and " << mesh->instance_data.size() << " faces" << std::endl;
//
//	return mesh;
//}

#include <bitset>

int Chunk::encodeFaceData(int x, int y, int z, int face, glm::vec3 color)// int texture_id)
{
	color *= 7.0f;
	int data = (x & 63);
	data |= (y & 63) << 6;
	data |= (z & 63) << 12;
	data |= (face & 7) << 18;

	data |= ((int)color.x & 7) << 21;
	data |= ((int)color.y & 7) << 24;
	data |= ((int)color.z & 7) << 27;


	//std::bitset<32> bits(data);
	//std::cout << "instance data: " << bits <<  ", " << data << " (" << x << ", " << y << ", " << z << ", " << face << ", " << texture_id << ")" << std::endl;
	return data;
}

void Chunk::drawInstanced(Mesh* mesh, Camera* camera, void* obj)
{
	Chunk* chunk = (Chunk*)obj;

	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat() * mesh->transform.getMat());
	mesh->shader->setFloat("unit_length", chunk->unit_length);

	switch (mesh->style)
	{
	case Shader::VAOStyle::TEXTURED:
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		break;

	case Shader::VAOStyle::SOLID_COLORED:
		mesh->shader->setVec3("color", mesh->color);
		break;
	}

	glBindVertexArray(mesh->vao->ID);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, mesh->verts.size(), mesh->instance_data.size());
}