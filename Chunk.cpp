#include "Chunk.h"

float Chunk::unit_length = 1;

Chunk::Chunk(int x, int y, int z) : x(x), y(y), z(z)
{
	this->ID = newID();
	std::stringstream buffer;
	buffer << "chunckmesh" << x << y << z;
	buffer >> mesh_name;

	for (int x = 0; x < CHUNK_SIZE; x++) for (int z = 0; z < CHUNK_SIZE; z++) for (int y = 0; y < CHUNK_SIZE; y++)
	{
		if (y < (2 * (sin(x * 6.0 / CHUNK_SIZE) * cos(z * 6.0 / CHUNK_SIZE)) + (CHUNK_SIZE / 2)))
			blocks[x][y][z] = BlockType::DIRT;
		else
			blocks[x][y][z] = BlockType::AIR;
	}
}

glm::vec3 Chunk::getPosf()
{
	return glm::vec3(x, y, z);
}

int Chunk::generateFaceData(std::vector<int>& data)
{
	int instances = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = CHUNK_SIZE - 1; y >= 0; y--)
			{
				if (blocks[x][y][z] != BlockType::AIR)
					continue;
				//top
				if (y != 0) if (blocks[x][y - 1][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 0, 1));
					instances++;
				}

				//bottom
				if (y != CHUNK_SIZE - 1) if (blocks[x][y + 1][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 1, 0));
					instances++;
				}

				//left
				if (x != 0) if (blocks[x - 1][y][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 2, 0));
					instances++;
				}

				//right
				if (x != CHUNK_SIZE - 1) if (blocks[x + 1][y][z] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 3, 0));
					instances++;
				}

				//forward
				if (z != 0) if (blocks[x][y][z - 1] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 4, 0));
					instances++;
				}

				//back
				if (z != CHUNK_SIZE - 1) if (blocks[x][y][z + 1] != BlockType::AIR)
				{
					data.push_back(encodeFaceData(x, y, z, 5, 0));
					instances++;
				}
			}
		}
	}
	return instances;
}

Mesh* Chunk::generateMesh()
{
	removeMesh(mesh_name);
	mesh = new Mesh(getTextureByName("chunk_texture"));
	meshes[mesh_name] = mesh;

	mesh->verts.push_back(glm::vec3(0, 0, 0));
	mesh->verts.push_back(glm::vec3(0, 0, 1));
	mesh->verts.push_back(glm::vec3(1, 0, 0));
	mesh->verts.push_back(glm::vec3(1, 0, 1));

	mesh->uv_coords.push_back(glm::vec2(0, 0));
	mesh->uv_coords.push_back(glm::vec2(0.5, 0));
	mesh->uv_coords.push_back(glm::vec2(0, 1));
	mesh->uv_coords.push_back(glm::vec2(0.5, 1));

	generateFaceData(mesh->instance_data);

	mesh->shader = getShaderByName("chunk_shader");
	mesh->generateInstancedVAO();
	mesh->drawFunction = drawInstanced;
	mesh->transform.translate(getPosf() * (float)CHUNK_SIZE * unit_length);

	//std::cout << mesh_name << " has " << mesh->verts.size() << " verts and " << mesh->instance_data.size() << " faces" << std::endl;

	return mesh;
}

#include <bitset>

int Chunk::encodeFaceData(int x, int y, int z, int face, int texture_id)
{
	int data = (x & 31);
	data |= (y & 31) << 5;
	data |= (z & 31) << 10;
	data |= (face & 7) << 15;
	data |= (texture_id & 1) << 18;

	//std::bitset<32> bits(data);
	//std::cout << "instance data: " << bits <<  ", " << data << " (" << x << ", " << y << ", " << z << ", " << face << ", " << texture_id << ")" << std::endl;
	return data;
}

void Chunk::drawInstanced(Mesh* mesh, Camera* camera)
{
	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat() * mesh->transform.getMat());
	//mesh->shader->setMat4("transform", mesh->transform.getMat());
	mesh->shader->setFloat("unit_length", unit_length);

	switch (mesh->style)
	{
	case Shader::VAOStyle::TEXTURED:
		glBindTexture(GL_TEXTURE_2D, mesh->texture);
		break;

	case Shader::VAOStyle::SOLID_COLORED:
		mesh->shader->setVec3("color", mesh->color);
		break;
	}

	//glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glBindVertexArray(mesh->VAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, mesh->verts.size(), mesh->instance_data.size());
}

int Chunk::newID()
{
	static int ID = 0;
	int id = ID++;
	return id;
}