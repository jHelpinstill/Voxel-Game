#include "Chunk.h"

Chunk::Chunk(int x, int y, int z) : x(x), y(y), z(z)
{
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

Mesh* Chunk::generateMesh(float block_size)
{
	removeMesh(mesh_name);
	mesh = new Mesh(getTextureByName("chunk_texture"));
	meshes[mesh_name] = mesh;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = CHUNK_SIZE - 1; y >= 0; y--)
			{
				glm::vec3 root = glm::vec3(x, y, z) * block_size + getPosf() * (float)CHUNK_SIZE * block_size;
				//top
				if (y != 0) if (blocks[x][y][z] == BlockType::AIR && blocks[x][y - 1][z] != BlockType::AIR)
					addQuad(root, 0, block_size);

				//bottom
				if (y != CHUNK_SIZE - 1) if (blocks[x][y][z] == BlockType::AIR && blocks[x][y + 1][z] != BlockType::AIR)
					addQuad(root, 1, block_size);

				//left
				if (x != 0) if (blocks[x][y][z] == BlockType::AIR && blocks[x - 1][y][z] != BlockType::AIR)
					addQuad(root, 2, block_size);

				//right
				if (x != CHUNK_SIZE - 1) if (blocks[x][y][z] == BlockType::AIR && blocks[x + 1][y][z] != BlockType::AIR)
					addQuad(root, 3, block_size);

				//forward
				if (z != 0) if (blocks[x][y][z] == BlockType::AIR && blocks[x][y][z - 1] != BlockType::AIR)
					addQuad(root, 4, block_size);

				//back
				if (z != CHUNK_SIZE - 1) if (blocks[x][y][z] == BlockType::AIR && blocks[x][y][z + 1] != BlockType::AIR)
					addQuad(root, 5, block_size);
			}
		}
	}

	mesh->attachShader(getShaderByName("chunk_shader"));
	return mesh;
}