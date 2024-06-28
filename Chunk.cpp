#include "Chunk.h"
#include "util.h"

Chunk::Chunk(int x, int y, int z, long seed, float unit_length)
	: faces_BVH(raycastFace, expandToFitFace)
	, x(x), y(y), z(z), seed(seed), unit_length(unit_length)
{
	this->ID = -1;

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

/*
* Scrubs through every block in the chunk in order and finds the visible faces.
* Visible faces occur only at the adjoining faces of a transparent block and an
* opaque block: as a face between two opaque blocks is obscured by the blocks,
* and obviously two transparent blocks have no visible faces.
*/
int Chunk::generateFaceData(std::vector<int>& data, Group neighboring_chunks)
{
	faces_BVH.reset();

	int instances = 0;
	std::srand(seed);
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = CHUNK_SIZE - 1; y >= 0; y--)
			{
				int rand_num = std::rand();
				if (blocks[x][y][z] != BlockType::AIR)
					continue;

				glm::vec3 pos = glm::vec3(x, y, z);

				glm::vec3 surrounding_block_positions[6] =
				{
					pos - util::Y, pos + util::Y,
					pos - util::X, pos + util::X,
					pos - util::Z, pos + util::Z
				};

				bool inside_boundaries[6] =
				{
					(y != 0), (y != CHUNK_SIZE - 1),
					(x != 0), (x != CHUNK_SIZE - 1),
					(z != 0), (z != CHUNK_SIZE - 1)
				};

				int surrounding_block_coords[6][3] =
				{
					{x, y - 1, z}, {x, y + 1, z},
					{x - 1, y, z}, {x + 1, y, z},
					{x, y, z - 1}, {x, y, z + 1}
				};

				int neighboring_chunks_block_coords[6][3] =
				{
					{x, CHUNK_SIZE - 1, z}, {x, 0, z},
					{CHUNK_SIZE - 1, y, z}, {0, y, z},
					{x, y, CHUNK_SIZE - 1}, {x, y, 0}
				};

				for (int dir = 0; dir < 6; dir++)
				{
					const BlockType* block = nullptr;

					if (inside_boundaries[dir])
						block = &blocks[surrounding_block_coords[dir][0]][surrounding_block_coords[dir][1]][surrounding_block_coords[dir][2]];
					else if (neighboring_chunks[dir])
						block = &neighboring_chunks[dir]->blocks[neighboring_chunks_block_coords[dir][0]][neighboring_chunks_block_coords[dir][1]][neighboring_chunks_block_coords[dir][2]];

					if (block && *block != BlockType::AIR)
					{
						data.push_back(encodeFaceData(x, y, z, dir, getBlockColor(*block, dir, rand_num)));
						faces_BVH.root->addDataNode(surrounding_block_positions[dir], dir);
						instances++;
					}
				}
			}
		}
	}

	faces_BVH.root->split(6);
	faces = instances;
	return instances;
}
/*
* x, y, z: 5 bits each
* face direction: 3 bits (6 directions)
* color: 3 bits per channel (rgb)
* Encoded data bits: 00000bbbgggrrrfffzzzzzyyyyyxxxxx
* 
* This encoder must be partnered with a corresponding decoder in the shader.
*/
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

	return data;
}

Chunk::RaycastResult Chunk::raycast(const glm::vec3& pos, const glm::vec3& ray)
{
	glm::vec3 chunk_space_pos = (pos - getPosf()) * (1.0f / unit_length);
	RaycastResult result = faces_BVH.raycast(chunk_space_pos, ray);
	if (result.hit)
		result.pos = getPosf() + result.pos * unit_length;
	return result;
}

bool Chunk::raycastFace(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3& face_pos, int* face)
{
	std::cout << "raycastFace called with face direction: " << *face << std::endl;
	Quad quad(face_pos, *face);
	return rayIntersectsPoly(pos, ray, quad.verts, 4, util::PolyCulling::CCW);
}

void Chunk::expandToFitFace(const glm::vec3& pos, const int& face, glm::vec3& min, glm::vec3& max)
{
	glm::vec3 face_min, face_max;
	switch (face)
	{
	case 0:
		face_min = pos + util::Y;
		face_max = pos + util::XYZ;
		break;
	case 1:
		face_min = pos;
		face_max = util::ZX;
		break;
	case 2:
		face_min = pos + util::X;
		face_max = pos + util::XYZ;
		break;
	case 3:
		face_min = pos;
		face_max = pos + util::YZ;
		break;
	case 4:
		face_min = pos + util::Z;
		face_max = pos + util::XYZ;
		break;
	case 5:
		face_min = pos;
		face_max = pos + util::XY;
		break;
	}
	min = glm::min(min, face_min);
	max = glm::max(max, face_max);
}

//////////////////// FUNCTION DEFINIIONS (GROUP) /////////////////////////	

Chunk::Group::Group(int size) : size(size)
{
	chunks = new Chunk* [size];
}

Chunk::Group::Group(const Group& other)
{
	this->size = other.size;
	for (int i = 0; i < size; i++)
		this->chunks[i] = other.chunks[i];
}

Chunk::Group::Group(Group&& other) noexcept
	: chunks(other.size ? other.chunks : nullptr)
	, size(other.size)
{
	other.chunks = nullptr;
	other.size = 0;
}

Chunk*& Chunk::Group::operator[](int i)
{
	return chunks[i];
}

Chunk::Group::~Group()
{
	delete chunks;
}