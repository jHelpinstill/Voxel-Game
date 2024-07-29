#pragma once
#ifndef CHUNK_
#define CHUNK_

#include "Config.h"
#include "ObjectManager.h"
#include "BlockType.h"
#include "Mesh.h"
#include "BVH.h"

constexpr auto CHUNK_SIZE = 32;	// number of blocks per side (DO NOT CHANGE)
constexpr auto CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr auto CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

// manages and organizes a group of blocks
class Chunk
{
public:
	struct ShaderInfo
	{
		int coord_bits;
		int face_bits;
		int color_bits;
	} shader_info;

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
	// Group allows the easiy manipulation of collections of chunks. 
	class Group
	{
	public:
		Chunk** chunks;
		int size;

		Group() : chunks(nullptr), size(0) {}	// default ctor (constructor)
		Group(int size);						// ctor with size
		Group(const Group& other);				// copy ctor
		Group(Group&& other) noexcept;			// move ctor

		Chunk*& operator[](int i);

		~Group();
	};

	struct Blocks
	{
		BlockType data[CHUNK_VOLUME];

		BlockType& operator[](int index);
		BlockType& operator()(int x, int y, int z);

		int getIndex(BlockType* block);
		bool getCoords(BlockType* block, int& x, int& y, int& z);
		bool onBoundary(BlockType* block, int* face = nullptr);

		BlockType* getNeighbor(BlockType* block, int face, int dist = 1);
	} blocks;

	int x, y, z, ID, faces;
	// x, y, z: the chunk's coordinates in the world in units of CHUNK_SIZE
	// ID: chunk's position in the worlds chunk_draw_params array (used for drawing world fron instanced quad)
	// faces: number of quad instances belonging to this chunk in the world mesh
	
	float unit_length;		// length of one block in world coordinates
	long seed;				// unique random number used for various things
	
	// face type containes pointer to parent block, and the direction of the face
	struct Face
	{
		BlockType* block;
		int norm;
	};
	BVH<Face> faces_BVH;	// Bounded Volume Hierarchy of faces is used to retrieve a block pointer through raycasting
	static bool raycastFace(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3& face_pos, Face* face);
	static void expandToFitFace(const glm::vec3& pos, Face* face, glm::vec3& min, glm::vec3& max);

	typedef BVH<Face>::RaycastResult RaycastResult;
	RaycastResult raycast(const glm::vec3& pos, const glm::vec3& ray);
	RaycastResult last_successful_raycast;
	
	Chunk(int x, int y, int z, long seed, ShaderInfo shader_info, float unit_length = 1);
	
	glm::vec3 getPosf();
	int generateFaceData(std::vector<int>& data, Group neighboring_chunks);
	int encodeFaceData(int x, int y, int z, int face, const glm::vec3& color);
};

// used for the hashing of chunks using the custom ChunkManager::Key
template <>
struct std::hash<Chunk::Key>
{
	std::size_t operator()(const Chunk::Key& k) const
	{
		using std::size_t;
		using std::hash;
		using std::string;

		return ((hash<int>()(k.x)
			^ (hash<int>()(k.y) << 1)) >> 1)
			^ (hash<int>()(k.z) << 1);
	}
};

#endif