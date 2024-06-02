#include "Chunk.h"

float Chunk::unit_length = 1;

Chunk::Chunk(int x, int y, int z) : x(x), y(y), z(z)
{
	std::stringstream buffer;
	buffer << "chunckmesh" << x << y << z;
	buffer >> mesh_name;

	for (int x = 0; x < CHUNK_SIZE; x++) for (int z = 0; z < CHUNK_SIZE; z++) for (int y = 0; y < CHUNK_SIZE; y++)
	{
		if(y < CHUNK_SIZE / 2)//if (y < (2 * (sin(x * 6.0 / CHUNK_SIZE) * cos(z * 6.0 / CHUNK_SIZE)) + (CHUNK_SIZE / 2)))
			blocks[x][y][z] = BlockType::DIRT;
		else
			blocks[x][y][z] = BlockType::AIR;
	}
}

glm::vec3 Chunk::getPosf()
{
	return glm::vec3(x, y, z);
}

void addQuadUV(std::vector<glm::vec2>& uv_coords, bool top_face = false)
{
	if (top_face)
	{
		uv_coords.push_back(glm::vec2(0.5, 0));
		uv_coords.push_back(glm::vec2(1, 0));
		uv_coords.push_back(glm::vec2(0.5, 1));
		uv_coords.push_back(glm::vec2(1, 0));
		uv_coords.push_back(glm::vec2(1, 1));
		uv_coords.push_back(glm::vec2(0.5, 1));
	}
	else
	{
		uv_coords.push_back(glm::vec2(0, 0));
		uv_coords.push_back(glm::vec2(0.5, 0));
		uv_coords.push_back(glm::vec2(0, 1));
		uv_coords.push_back(glm::vec2(0.5, 0));
		uv_coords.push_back(glm::vec2(0.5, 1));
		uv_coords.push_back(glm::vec2(0, 1));
	}

}

void Chunk::addQuad(glm::vec3 root, int face, float length)
{
	/*
		face:
		0: up
		1: down
		2: left
		3: right
		4: forward
		5: back
	*/

	if (!mesh)
		return;

	glm::vec3 v[4];

	switch (face)
	{
	case 0:	// up (+y)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(0, 0, 0);
		v[1].z += length;
		v[2].x += length; v[2].z += length;
		v[3].x += length;
		break;
	case 1:	// down (-y)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(0, length, length);
		v[1].z -= length;
		v[2].x += length; v[2].z -= length;
		v[3].x += length;
		break;
	case 2:	// left (+x)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(0, 0, 0);
		v[1].y += length;
		v[2].y += length; v[2].z += length;
		v[3].z += length;
		break;
	case 3:	// right (-x)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(length, 0, length);
		v[1].y += length;
		v[2].y += length; v[2].z -= length;
		v[3].z -= length;
		break;
	case 4:	// forward (+z)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(length, 0, 0);
		v[1].y += length;
		v[2].y += length; v[2].x -= length;
		v[3].x -= length;
		break;
	case 5:	// backward (-z)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(0, 0, length);
		v[1].y += length;
		v[2].y += length; v[2].x += length;
		v[3].x += length;
		break;
	}


	mesh->verts.push_back(v[0]);
	mesh->verts.push_back(v[1]);
	mesh->verts.push_back(v[3]);
	mesh->verts.push_back(v[1]);
	mesh->verts.push_back(v[2]);
	mesh->verts.push_back(v[3]);

	// add uv coords for quad
	if (face == 0)
	{
		mesh->uv_coords.push_back(glm::vec2(0.5, 0));
		mesh->uv_coords.push_back(glm::vec2(1, 0));
		mesh->uv_coords.push_back(glm::vec2(0.5, 1));
		mesh->uv_coords.push_back(glm::vec2(1, 0));
		mesh->uv_coords.push_back(glm::vec2(1, 1));
		mesh->uv_coords.push_back(glm::vec2(0.5, 1));
	}
	else
	{
		mesh->uv_coords.push_back(glm::vec2(0, 0));
		mesh->uv_coords.push_back(glm::vec2(0.5, 0));
		mesh->uv_coords.push_back(glm::vec2(0, 1));
		mesh->uv_coords.push_back(glm::vec2(0.5, 0));
		mesh->uv_coords.push_back(glm::vec2(0.5, 1));
		mesh->uv_coords.push_back(glm::vec2(0, 1));
	}
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
					mesh->instance_data.push_back((int)generateVertexData(x, y, z, 0, 1));

				//bottom
				if (y != CHUNK_SIZE - 1) if (blocks[x][y + 1][z] != BlockType::AIR)
					mesh->instance_data.push_back((int)generateVertexData(x, y, z, 1, 0));

				//left
				if (x != 0) if (blocks[x - 1][y][z] != BlockType::AIR)
					mesh->instance_data.push_back((int)generateVertexData(x, y, z, 2, 0));

				//right
				if (x != CHUNK_SIZE - 1) if (blocks[x + 1][y][z] != BlockType::AIR)
					mesh->instance_data.push_back((int)generateVertexData(x, y, z, 3, 0));

				//forward
				if (z != 0) if (blocks[x][y][z - 1] != BlockType::AIR)
					mesh->instance_data.push_back((int)generateVertexData(x, y, z, 4, 0));

				//back
				if (z != CHUNK_SIZE - 1) if (blocks[x][y][z + 1] != BlockType::AIR)
					mesh->instance_data.push_back((int)generateVertexData(x, y, z, 5, 0));
			}
		}
	}

	mesh->shader = getShaderByName("chunk_shader");
	mesh->generateInstancedVAO();
	mesh->drawFunction = drawInstanced;
	mesh->transform.translate(getPosf() * (float)CHUNK_SIZE * unit_length);

	std::cout << mesh_name << " has " << mesh->verts.size() << " verts and " << mesh->instance_data.size() << " faces" << std::endl;

	return mesh;
}

#include <bitset>

int Chunk::generateVertexData(int x, int y, int z, int face, int texture_id)
{
	int data = (x & 31);
	data |= (y & 31) << 5;
	data |= (z & 31) << 10;
	data |= (face & 7) << 15;
	data |= (texture_id & 1) << 18;

	std::bitset<32> bits(data);
	std::cout << "instance data: " << bits << " (" << x << ", " << y << ", " << z << ", " << face << ", " << texture_id << ")" << std::endl;
	return data;
}

void Chunk::drawInstanced(Mesh* mesh, Camera* camera)
{
	mesh->shader->use();
	mesh->shader->setMat4("projection", camera->getProjectionMat());
	mesh->shader->setMat4("transform", mesh->transform.getMat());
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