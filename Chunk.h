#pragma once
#ifndef CHUNK_
#define CHUNK_

#include "config.h"
#include "ObjectManager.h"
#include "BlockType.h"
#include "Mesh.h"
#include "BVH.h"

constexpr auto CHUNK_SIZE = 64;

class Chunk
{
public:
	BlockType blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]{};
	int x, y, z, ID, faces;

	struct Face
	{
		BlockType* block;
		int normal;
	};
	BVH<Face*> FacesBVH;

	Mesh* mesh;
	std::string mesh_name;
	float unit_length;
	long seed;

	Chunk(int x, int y, int z, long seed, float unit_length = 1);
	
	glm::vec3 getPosf();
	//Mesh* generateMesh();
	int generateFaceData(std::vector<int>& data);
	int encodeFaceData(int x, int y, int z, int face, glm::vec3 color);// int texture_id);

	static void drawInstanced(Mesh* mesh, Camera* camera, void* obj);
};

struct ChunkKey
{
	int x, y, z;
	ChunkKey(int x, int y, int z) : x(x), y(y), z(z) {}
	bool operator==(const ChunkKey& other) const
	{
		return (x == other.x
			&& y == other.y
			&& z == other.z);
	}
};

template <>
struct std::hash<ChunkKey>
{
	std::size_t operator()(const ChunkKey& k) const
	{
		using std::size_t;
		using std::hash;
		using std::string;

		// Compute individual hash values for first,
		// second and third and combine them using XOR
		// and bit shifting:

		return ((hash<int>()(k.x)
			^ (hash<int>()(k.y) << 1)) >> 1)
			^ (hash<int>()(k.z) << 1);
	}
};

#endif