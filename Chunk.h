#pragma once
#ifndef CHUNK_
#define CHUNK_

#include "config.h"
#include "ObjectManager.h"
#include "BlockType.h"
#include "Mesh.h"
#include "BVH.h"

constexpr auto CHUNK_SIZE = 32;

class Chunk
{
public:
	class Group
	{
	public:
		Chunk** chunks;
		int size;

		Group() : chunks(nullptr), size(0) {}
		Group(int size);
		Group(const Group& other);
		Group(Group&& other) noexcept;

		Chunk*& operator[](int i);

		~Group();
	};
	struct Key
	{
		int x, y, z;
		Key(int x, int y, int z) : x(x), y(y), z(z) {}
		bool operator==(const Key& other) const
		{
			return (x == other.x
				&& y == other.y
				&& z == other.z);
		}
	};

	BlockType blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]{};
	int x, y, z, ID, faces;

	Mesh* mesh;
	std::string mesh_name;
	float unit_length;
	long seed;

	BVH<int> faces_BVH;

	Chunk(int x, int y, int z, long seed, float unit_length = 1);
	
	glm::vec3 getPosf();
	//Mesh* generateMesh();
	int generateFaceData(std::vector<int>& data, Group neighboring_chunks);
	int encodeFaceData(int x, int y, int z, int face, glm::vec3 color);// int texture_id);

	static void drawInstanced(Mesh* mesh, Camera* camera, void* obj);
};

template <>
struct std::hash<Chunk::Key>
{
	std::size_t operator()(const Chunk::Key& k) const
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