#include "Chunk.h"
#include "ChunkManager.h"
#include "util.h"
#include "Debug.h"
#include <algorithm>
#include <numeric>

Chunk::Chunk(int x, int y, int z, long seed, ShaderInfo shader_info, float unit_length)
	: faces_BVH(expandToFitFace, 6)
	, x(x), y(y), z(z), seed(seed), unit_length(unit_length), shader_info(shader_info) {
	this->ID = -1;

	for (int xb = 0; xb < CHUNK_SIZE; xb++) for (int zb = 0; zb < CHUNK_SIZE; zb++) for (int yb = 0; yb < CHUNK_SIZE; yb++) {
		glm::vec3 pos = getPosf();
		pos.x += xb * unit_length;
		pos.y += yb * unit_length;
		pos.z += zb * unit_length;

		if (pos.y < 2.0) {
			blocks(xb, yb, zb) = BlockType::STONE;
			continue;
		}
		if (pos.y < 
			3.2 +
			2.1 * (sin(pos.x * 3.0 / 64) * cos(pos.z * 7.0 / 64)) +
			0.5 * (sin(pos.x * 25.0 / 64) * cos(pos.z * 29.0 / 64))
			)
			blocks(xb, yb, zb) = BlockType::DIRT;
		else {
			for(yb; yb < CHUNK_SIZE; yb++)
				blocks(xb, yb, zb) = BlockType::AIR;
		}
	}
}

// returns the chunk's position in world space, rather than chunk coordinates.
glm::vec3 Chunk::getPosf() {
	return glm::vec3(x, y, z) * (float)CHUNK_SIZE * unit_length;
}

bool Chunk::setBlock(BlockType *block, BlockType new_type) {
	bool success = false;
	if(blocks.isValid(block)) {
		*block = new_type;
		modified = true;
		success = true;
	}
	return success;
}

bool Chunk::setBlock(int x, int y, int z, BlockType new_type) {
	bool success = false;
	if(x >= 0 && x < 32 && y >= 0 && y < 32 && z >= 0 && z < 32) {
		success = setBlock(&blocks(x, y, z), new_type);
	}
	return success;
}

/*
* Checks through every block in the chunk and finds visible faces.
* Visible faces occur only at the adjoining faces of a transparent block and an
* opaque block: as a face between two opaque blocks is obscured by the blocks,
* and obviously two transparent blocks have no visible faces.
*/
int Chunk::generateFaceData(std::vector<int> &data, Group neighboring_chunks) {
	faces_BVH.reset();

	int instances = 0;
	std::srand(seed);
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = CHUNK_SIZE - 1; y >= 0; y--) {
				int rand_num = std::rand();
				if (blocks(x, y, z) != BlockType::AIR)
					continue;
				
				bool inside_boundaries[6] = {
					(y != 0), (y != CHUNK_SIZE - 1),
					(x != 0), (x != CHUNK_SIZE - 1),
					(z != 0), (z != CHUNK_SIZE - 1)
				};
				for (int dir = 0; dir < 6; dir++) {
					BlockType *block = nullptr;
					if (inside_boundaries[dir]) {
						int surrounding_block_coords[3] = {x, y, z};
						switch(dir) {
							case 0: surrounding_block_coords[1]--; break;
							case 1: surrounding_block_coords[1]++; break;
							case 2: surrounding_block_coords[0]--; break;
							case 3: surrounding_block_coords[0]++; break;
							case 4: surrounding_block_coords[2]--; break;
							case 5: surrounding_block_coords[2]++; break;
						}
						block = &blocks(surrounding_block_coords[0], surrounding_block_coords[1], surrounding_block_coords[2]);
					}
					else if (neighboring_chunks[dir]) {
						int neighboring_chunks_block_coords[3] = {x, y, z};
						switch(dir) {
							case 0: neighboring_chunks_block_coords[1] = CHUNK_SIZE - 1; break;
							case 1: neighboring_chunks_block_coords[1] = 0; break;
							case 2: neighboring_chunks_block_coords[0] = CHUNK_SIZE - 1; break;
							case 3: neighboring_chunks_block_coords[0] = 0; break;
							case 4: neighboring_chunks_block_coords[2] = CHUNK_SIZE - 1; break;
							case 5: neighboring_chunks_block_coords[2] = 0; break;
						}
						block = &neighboring_chunks[dir]->blocks(neighboring_chunks_block_coords[0], neighboring_chunks_block_coords[1], neighboring_chunks_block_coords[2]);
					}
					if (block && *block != BlockType::AIR) {
						data.push_back(encodeFaceData(x, y, z, dir, getBlockColor(*block, dir, rand_num)));
						Face face = { block, dir };
						glm::vec3 pos = glm::vec3(x, y, z);
						switch(dir) {
							case 0: pos = pos - util::Y; break;
							case 1: pos = pos + util::Y; break;
							case 2: pos = pos - util::X; break;
							case 3: pos = pos + util::X; break;
							case 4: pos = pos - util::Z; break;
							case 5: pos = pos + util::Z; break;
						}
						faces_BVH.root->createDataNode(pos, face);
						instances++;
					}
				}
			}
		}
	}

	modified = false;
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
* Color param is an int vector that must give the color value directly for each channel. 
*/
int Chunk::encodeFaceData(int x, int y, int z, int face, const glm::ivec3 &color) { // int texture_id)
	const int coord_mask = (1 << shader_info.coord_bits) - 1;
	const int face_mask = (1 << shader_info.face_bits) - 1;
	const int color_mask = (1 << shader_info.color_bits) - 1;
	// int offset = 0;
	int data = 0;

	data |= (x & coord_mask) << shader_info.getCoordBitPos(0);
	data |= (y & coord_mask) << shader_info.getCoordBitPos(1);
	data |= (z & coord_mask) << shader_info.getCoordBitPos(2);
	
	data |= (face & face_mask) << shader_info.getFaceBitPos();

	data |= (color.x & color_mask) << shader_info.getColorBitPos(0);
	data |= (color.y & color_mask) << shader_info.getColorBitPos(1);
	data |= (color.z & color_mask) << shader_info.getColorBitPos(2);

	return data;
}

Chunk::RaycastResult Chunk::raycast(const glm::vec3 &pos, const glm::vec3 &ray) {
	RaycastResult result{};
	std::vector<BVH<Face>::DataNode*> hits, sorted_hits;
	glm::vec3 chunk_space_pos = (pos - getPosf()) * (1.0f / unit_length);
	faces_BVH.raycast(chunk_space_pos, ray, hits);
	if(hits.size()) {
		std::vector<int> indices(hits.size());
		std::iota(indices.begin(), indices.end(), 0);
		std::sort(indices.begin(), indices.end(), [&](const int a, const int b) {
			return glm::length(hits[a]->pos - chunk_space_pos) < glm::length(hits[b]->pos - chunk_space_pos);
		});
		glm::vec3 posf = getPosf();
		for(int i = 0; i < indices.size(); i++) {
			sorted_hits.push_back(hits[indices[i]]);
		}
		for(auto &hit : sorted_hits) {
			if(raycastFace(chunk_space_pos, ray, hit->pos, &hit->obj)) {
				result.hit = true;
				result.obj = &hit->obj;
				result.pos = posf + (hit->pos * unit_length);
				break;
			}
		}
	}
	return result;
}

bool Chunk::raycastFace(const glm::vec3 &pos, const glm::vec3 &ray, const glm::vec3 &face_pos, Face *face) {
	//td::cout << "raycastFace called with face direction: " << *face << std::endl;
	Quad quad(face_pos, face->norm);
	return rayIntersectsPoly(pos, ray, quad.verts, 4, util::PolyCulling::CCW);
}

void Chunk::expandToFitFace(const glm::vec3 &pos, Face *face, glm::vec3 &min, glm::vec3 &max) {
	glm::vec3 face_min, face_max;
	switch (face->norm) {
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

Chunk::Group::Group(int size) : size(size) {
	chunks = new Chunk * [size];
}

Chunk::Group::Group(const Group &other) {
	this->size = other.size;
	for (int i = 0; i < size; i++)
		this->chunks[i] = other.chunks[i];
}

Chunk::Group::Group(Group &&other) noexcept
	: chunks(other.size ? other.chunks : nullptr)
	, size(other.size) {
	other.chunks = nullptr;
	other.size = 0;
}

Chunk *&Chunk::Group::operator[](int i) {
	return chunks[i];
}

Chunk::Group::~Group() {
	delete chunks;
}

//////////////////// FUNCTION DEFINIIONS (Blocks) /////////////////////////

BlockType &Chunk::Blocks::operator[](int index) {
	return data[index];
}

BlockType &Chunk::Blocks::operator()(int x, int y, int z) {
	return data[x + CHUNK_SIZE * y + CHUNK_AREA * z];
}

bool Chunk::Blocks::isValid(BlockType *block) {
	int index = block - data;
	bool valid = false;
	if(index >= 0 && index < CHUNK_VOLUME)
		valid = true;
	return valid;	
}

int Chunk::Blocks::getIndex(BlockType *block) {
	if(!block)
		return -1;
	return (block - data);
}

bool Chunk::Blocks::getCoords(BlockType *block, int &x, int &y, int &z) {
	x = y = z = -1;
	if(!block)
		return false;
		
	int i = getIndex(block);
	if (i < 0 || i >= CHUNK_VOLUME)
		return false;

	x = i % CHUNK_SIZE;
	y = (i % CHUNK_AREA) / CHUNK_SIZE;
	z = i / CHUNK_AREA;
	return true;
}

bool Chunk::Blocks::onBoundary(BlockType *block, std::vector<int> &faces) {
	int x, y, z;
	bool on_boundary = false;
	if (getCoords(block, x, y, z)) {
		if (y == 31) {
			faces.push_back(0);
			on_boundary = true;
		}
		if (y == 0) {
			faces.push_back(1);
			on_boundary = true;
		}
		if (x == 31) {
			faces.push_back(2);
			on_boundary = true;
		}
		if (x == 0) {
			faces.push_back(3);
			on_boundary = true;
		}
		if (z == 31) {
			faces.push_back(4);
			on_boundary = true;
		}
		if (z == 0) {
			faces.push_back(5);
			on_boundary = true;
		}
	}
	return on_boundary;
}

BlockType *Chunk::Blocks::getNeighbor(BlockType *block, int face, int dist) {
	int x, y, z;
	if (!getCoords(block, x, y, z))
		return nullptr;
	switch (face) {
		case 0: y++; if (y > 31) return nullptr; break;
		case 1: y--; if (y < 0) return nullptr; break;
		case 2: x++; if (x > 31) return nullptr; break;
		case 3: x--; if (x < 0) return nullptr; break;
		case 4: z++; if (z > 31) return nullptr; break;
		case 5: z--; if (z < 0) return nullptr; break;
	}

	return &(*this)(x, y, z);
}

int getXPos(int coord_bits, int face_bits, int color_bits) { return 0;}
int getYPos(int coord_bits, int face_bits, int color_bits) { return coord_bits;}
int getZPos(int coord_bits, int face_bits, int color_bits) { return coord_bits * 2;}
int getRPos(int coord_bits, int face_bits, int color_bits) { return coord_bits * 3 + face_bits;}
int getGPos(int coord_bits, int face_bits, int color_bits) { return coord_bits * 3 + face_bits + color_bits;}
int getBPos(int coord_bits, int face_bits, int color_bits) { return coord_bits * 3 + face_bits + color_bits * 2;}

typedef int (*ShaderInfoGetter)(int, int, int);

ShaderInfoGetter coordBitPos[3] = {getXPos, getYPos, getZPos};
ShaderInfoGetter colorBitPos[3] = {getRPos, getGPos, getBPos};

int Chunk::ShaderInfo::getCoordBitPos(int i) {
	return coordBitPos[i](coord_bits, face_bits, color_bits);
}
int Chunk::ShaderInfo::getFaceBitPos() {
	return coord_bits * 3;
}
int Chunk::ShaderInfo::getColorBitPos(int i) {
	return colorBitPos[i](coord_bits, face_bits, color_bits);
}
int Chunk::ShaderInfo::getCoordBitMask() {
	return (1 << coord_bits) - 1;
}
int Chunk::ShaderInfo::getFaceBitMask() {
	return (1 << face_bits) - 1;
}
int Chunk::ShaderInfo::getColorBitMask() {
	return (1 << color_bits) - 1;
}