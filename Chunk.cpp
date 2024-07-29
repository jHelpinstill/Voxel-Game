#include "Chunk.h"
#include "ChunkManager.h"
#include "util.h"

Chunk::Chunk(int x, int y, int z, long seed, ShaderInfo shader_info, float unit_length)
	: faces_BVH(raycastFace, expandToFitFace, 6)
	, x(x), y(y), z(z), seed(seed), unit_length(unit_length), shader_info(shader_info)
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
			blocks(xb, yb, zb) = BlockType::STONE;
			continue;
		}
		if (pos.y < 
			3.2 +
			2.1 * (sin(pos.x * 3.0 / 64) * cos(pos.z * 7.0 / 64)) +
			0.5 * (sin(pos.x * 25.0 / 64) * cos(pos.z * 29.0 / 64))
			)
			blocks(xb, yb, zb) = BlockType::DIRT;
		else
		{
			for(yb; yb < CHUNK_SIZE; yb++)
				blocks(xb, yb, zb) = BlockType::AIR;
		}
	}
}

// returns the chunk's position in world space, rather than chunk coordinates.
glm::vec3 Chunk::getPosf()
{
	return glm::vec3(x, y, z) * (float)CHUNK_SIZE * unit_length;
}

/*
* Checks through every block in the chunk and finds visible faces.
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
				if (blocks(x, y, z) != BlockType::AIR)
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
					BlockType* block = nullptr;

					if (inside_boundaries[dir])
						block = &blocks(surrounding_block_coords[dir][0], surrounding_block_coords[dir][1], surrounding_block_coords[dir][2]);
					else if (neighboring_chunks[dir])
						block = &neighboring_chunks[dir]->blocks(neighboring_chunks_block_coords[dir][0], neighboring_chunks_block_coords[dir][1], neighboring_chunks_block_coords[dir][2]);

					if (block && *block != BlockType::AIR)
					{
						data.push_back(encodeFaceData(x, y, z, dir, getBlockColor(*block, dir, rand_num)));
						Face face = { block, dir };
						faces_BVH.root->addDataNode(surrounding_block_positions[dir], face);
						instances++;
					}
				}
			}
		}
	}

	faces_BVH.build();
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
* 
* Encodes info about a face into a single 32-bit int. x, y, z are in block coordinates within the chunk
* (i.e. 0 -> CHUNK_SIZE - 1), face is the normal direction in standard format (0, 1, 2, 3, 4, 5 -> up, down, left, right, forward, back).
* Color param is a float vector, but is convereted into a reduced-bit version when encoded (3 bits per channel currently). 
*/
int Chunk::encodeFaceData(int x, int y, int z, int face, const glm::vec3& color)// int texture_id)
{
	const int coord_mask = (1 << shader_info.coord_bits) - 1;
	const int face_mask = (1 << shader_info.face_bits) - 1;
	const int color_mask = (1 << shader_info.color_bits) - 1;
	
	glm::vec3 reduced_color = color * (float)(color_mask + 1);
	int offset = 0;
	int data = 0;

	data |= (x & coord_mask) << offset; (offset += shader_info.coord_bits);
	data |= (y & coord_mask) << offset; (offset += shader_info.coord_bits);
	data |= (z & coord_mask) << offset; (offset += shader_info.coord_bits);

	data |= (face & face_mask) << offset; (offset += shader_info.face_bits);

	data |= ((int)reduced_color.x & color_mask) << offset; (offset += shader_info.color_bits);
	data |= ((int)reduced_color.y & color_mask) << offset; (offset += shader_info.color_bits);
	data |= ((int)reduced_color.z & color_mask) << offset; (offset += shader_info.color_bits);

	return data;
}

Chunk::RaycastResult Chunk::raycast(const glm::vec3& pos, const glm::vec3& ray)
{
	glm::vec3 chunk_space_pos = (pos - getPosf()) * (1.0f / unit_length);
	RaycastResult result = faces_BVH.raycast(chunk_space_pos, ray);
	if (result.hit)
	{
		result.pos = getPosf() + result.pos * unit_length;
		last_successful_raycast = result;
	}
	return result;
}

bool Chunk::raycastFace(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3& face_pos, Face* face)
{
	//td::cout << "raycastFace called with face direction: " << *face << std::endl;
	Quad quad(face_pos, face->norm);
	return rayIntersectsPoly(pos, ray, quad.verts, 4, util::PolyCulling::CCW);
}

void Chunk::expandToFitFace(const glm::vec3& pos, Face* face, glm::vec3& min, glm::vec3& max)
{
	glm::vec3 face_min, face_max;
	switch (face->norm)
	{
	case 0:
		face_min = pos + util::Y;
		face_max = pos + util::XYZ;
		break;
	case 1:
		face_min = pos;
		face_max = pos + util::ZX;
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
	chunks = new Chunk * [size];
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

//////////////////// FUNCTION DEFINIIONS (Blocks) /////////////////////////

BlockType& Chunk::Blocks::operator[](int index)
{
	return data[index];
}

BlockType& Chunk::Blocks::operator()(int x, int y, int z)
{
	return data[x + CHUNK_SIZE * y + CHUNK_AREA * z];
}

int Chunk::Blocks::getIndex(BlockType* block)
{
	return ((int)block - (int)data) / sizeof(BlockType);
}

bool Chunk::Blocks::getCoords(BlockType* block, int& x, int& y, int& z)
{
	int i = getIndex(block);
	if (i < 0 || i >= CHUNK_VOLUME)
		return false;

	x = i % CHUNK_SIZE;
	y = (i % CHUNK_AREA) / CHUNK_SIZE;
	z = i / CHUNK_AREA;
	return true;
}

bool Chunk::Blocks::onBoundary(BlockType* block, int* face)
{
	int x, y, z;
	if (!getCoords(block, x, y, z))
		return true;

	if (y == 31)
	{
		if (face)
			*face = 0;
		return true;
	}
	if (y == 0)
	{
		if (face)
			*face = 1;
		return true;
	}
	if (x == 31)
	{
		if (face)
			*face = 2;
		return true;
	}
	if (x == 0)
	{
		if (face)
			*face = 3;
		return true;
	}
	if (z == 31)
	{
		if (face)
			*face = 4;
		return true;
	}
	if (z == 0)
	{
		if (face)
			*face = 5;
		return true;
	}

	return false;
}

BlockType* Chunk::Blocks::getNeighbor(BlockType* block, int face, int dist)
{
	int x, y, z;
	if (!getCoords(block, x, y, z))
		return nullptr;
	switch (face)
	{
	case 0: y++; if (y > 31) return nullptr; break;
	case 1: y--; if (y < 0) return nullptr; break;
	case 2: x++; if (x > 31) return nullptr; break;
	case 3: x--; if (x < 0) return nullptr; break;
	case 4: z++; if (z > 31) return nullptr; break;
	case 5: z--; if (z < 0) return nullptr; break;
	}

	return &(*this)(x, y, z);
}