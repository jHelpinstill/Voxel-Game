#include "World.h"

void World::setup()
{
	chunks.push_back(new Chunk(glm::ivec3(0, 0, 0)));
	for (int r = 1; r < max_chunk_radius; r++)
	{
		int x, z;

		x = z = r;
		for (x; x <= r; x++)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
		for (z; z >= -r; z--)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
		for (x; x >= -r; x--)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
		for (z; z < r; z++)
			chunks.push_back(new Chunk(glm::ivec3(x, 0, z)));
	}

	generateMesh();
}

void addQuadUV(std::vector<glm::vec2>& uv_coords)
{
	uv_coords.push_back(glm::vec2(0, 0));
	uv_coords.push_back(glm::vec2(0.5, 0));
	uv_coords.push_back(glm::vec2(0, 1));
	uv_coords.push_back(glm::vec2(0.5, 0)); 
	uv_coords.push_back(glm::vec2(0.5, 1));
	uv_coords.push_back(glm::vec2(0, 1));
}

void addQuad(std::vector<glm::vec3>& verts, glm::vec3 root, int face, float length)
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

	glm::vec3 v[4];

	switch (face)
	{
	case 0:	// up (+y)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(0, length, 0);
		v[1].z += length;
		v[2].x += length; v[2].z += length;
		v[3].x += length;
		break;
	case 1:	// down (-y)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(0, 0, length);
		v[1].z -= length;
		v[2].x += length; v[2].z -= length;
		v[3].x += length;
		break;
	case 2:	// left (+x)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(length, 0, 0);
		v[1].y += length;
		v[2].y += length; v[2].z += length;
		v[3].z += length;
		break;
	case 3:	// right (-x)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(0, 0, length);
		v[1].y += length;
		v[2].y += length; v[2].z -= length;
		v[3].z -= length;
		break;
	case 4:	// forward (+z)
		v[0] = v[1] = v[2] = v[3] = root + glm::vec3(length, 0, length);
		v[1].y += length;
		v[2].y += length; v[2].x -= length;
		v[3].x -= length;
		break;
	case 5:	// backward (-z)
		v[0] = v[1] = v[2] = v[3] = root;
		v[1].y += length;
		v[2].y += length; v[2].x += length;
		v[3].x += length;
		break;
	}
	

	verts.push_back(v[0]);
	verts.push_back(v[1]);
	verts.push_back(v[3]);
	verts.push_back(v[1]);
	verts.push_back(v[2]);
	verts.push_back(v[3]);
}

void World::generateMesh()
{
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uv_coords;

	// top
	for (Chunk* chunk : chunks)
	{
		/// top
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				for (int y = CHUNK_SIZE - 1; y >= 0; y--)
				{
					//top
					if(y != 0) if (chunk->blocks[x][y][z] == BlockType::AIR && chunk->blocks[x][y - 1][z] != BlockType::AIR)
					{
						glm::vec3 root = glm::vec3(x, y, z) * chunk_unit_dimension + (glm::vec3)chunk->pos;
						addQuad(verts, root, 0, chunk_unit_dimension);
						addQuadUV(uv_coords);
					}
					//bottom
					if (y != CHUNK_SIZE - 1) if (chunk->blocks[x][y][z] == BlockType::AIR && chunk->blocks[x][y + 1][z] != BlockType::AIR)
					{
						glm::vec3 root = glm::vec3(x, y, z) * chunk_unit_dimension + (glm::vec3)chunk->pos;
						addQuad(verts, root, 1, chunk_unit_dimension);
						addQuadUV(uv_coords);
					}
					//left
					if (x != 0) if (chunk->blocks[x][y][z] == BlockType::AIR && chunk->blocks[x - 1][y][z] != BlockType::AIR)
					{
						glm::vec3 root = glm::vec3(x, y, z) * chunk_unit_dimension + (glm::vec3)chunk->pos;
						addQuad(verts, root, 2, chunk_unit_dimension);
						addQuadUV(uv_coords);
					}
					//right
					if (x != CHUNK_SIZE - 1) if (chunk->blocks[x][y][z] == BlockType::AIR && chunk->blocks[x + 1][y][z] != BlockType::AIR)
					{
						glm::vec3 root = glm::vec3(x, y, z) * chunk_unit_dimension + (glm::vec3)chunk->pos;
						addQuad(verts, root, 3, chunk_unit_dimension);
						addQuadUV(uv_coords);
					}
					//forward
					if (z != 0) if (chunk->blocks[x][y][z] == BlockType::AIR && chunk->blocks[x][y][z - 1] != BlockType::AIR)
					{
						glm::vec3 root = glm::vec3(x, y, z) * chunk_unit_dimension + (glm::vec3)chunk->pos;
						addQuad(verts, root, 4, chunk_unit_dimension);
						addQuadUV(uv_coords);
					}
					//back
					if (z != CHUNK_SIZE - 1) if (chunk->blocks[x][y][z] == BlockType::AIR && chunk->blocks[x][y][z + 1] != BlockType::AIR)
					{
						glm::vec3 root = glm::vec3(x, y, z) * chunk_unit_dimension + (glm::vec3)chunk->pos;
						addQuad(verts, root, 5, chunk_unit_dimension);
						addQuadUV(uv_coords);
					}
				}
			}
		}
	}

	Mesh* world_mesh = new Mesh("world_mesh", verts, getTextureByName("dirt_block"), uv_coords);
	removeMesh("world_mesh");
	meshes["world_mesh"] = world_mesh;
}