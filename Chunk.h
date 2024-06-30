#pragma once
#ifndef CHUNK_
#define CHUNK_

#include "Config.h"
#include "ObjectManager.h"
#include "BlockType.h"
#include "Mesh.h"
#include "BVH.h"

constexpr auto CHUNK_SIZE = 32;	// number of blocks per side
constexpr auto CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr auto CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

/*
* The chunk class manages a group of blocks
*/
class Chunk
{
public:
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
		BlockType& operator[](int index)
		{
			return data[index];
		}
		BlockType& operator()(int x, int y, int z)
		{
			return data[x + CHUNK_SIZE * y + CHUNK_AREA * z];
		}
		int getIndex(BlockType* block)
		{
			return ((int)block - (int)data) / sizeof(BlockType);
		}
		bool getCoords(BlockType* block, int& x, int& y, int& z)
		{
			int i = getIndex(block);
			if (i < 0 || i >= CHUNK_VOLUME)
				return false;

			x = i % CHUNK_SIZE;
			y = (i % CHUNK_AREA) / CHUNK_SIZE;
			z = i / CHUNK_AREA;
			return true;
		}
		bool onBoundary(BlockType* block, int* face = nullptr)
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
		BlockType* getNeighbor(BlockType* block, int face, int dist = 1)
		{
			int x, y, z;
			if (!getCoords(block, x, y, z))
				return nullptr;
			switch (face)
			{
			case 0: y++; if(y > 31) return nullptr; break;
			case 1: y--; if(y < 0) return nullptr; break;
			case 2: x++; if(x > 31) return nullptr; break;
			case 3: x--; if(x < 0) return nullptr; break;
			case 4: z++; if(z > 31) return nullptr; break;
			case 5: z--; if(z < 0) return nullptr; break;
			}

			return &(*this)(x, y, z);


			//int i = getIndex(block);
			//
			//std::cout << "index of block is: " << i << ", coords: " << i % CHUNK_SIZE << ", " << (i % CHUNK_AREA) / CHUNK_SIZE << ", " << i / CHUNK_AREA << std::endl;
			//switch (face)
			//{
			//case 0: i += CHUNK_SIZE * dist; break;
			//case 1: i -= CHUNK_SIZE * dist; break;
			//case 2: i += dist; break;
			//case 3: i -= dist; break;
			//case 4: i += CHUNK_AREA * dist; break;
			//case 5: i -= CHUNK_AREA * dist; break;
			//}
			//if (i < 0 || i >= CHUNK_VOLUME)
			//	return nullptr;
			//return &data[i];
		}
	} blocks;

	int x, y, z, ID, faces;
	/* 
	* x, y, z: the chunk's coordinates in the world in units of CHUNK_SIZE
	* ID: chunk's position in the worlds chunk_draw_params array (used for drawing world fron instanced quad)
	* faces: number of quad instances belonging to this chunk in world mesh
	*/
	
	float unit_length;		// length of one block in world coordinates
	long seed;				// unique random number used for various things

	/*
	* Bounding Volume Hierarchy used for raycasting. Enables the quick discovery of
	* which face in the world mesh (belonging to this chunk) is intersected by a ray.
	* Works similarly to a binary search, only in 3D space.
	*/
	
	struct Face
	{
		BlockType* block;
		int norm;
	};
	BVH<Face> faces_BVH;
	static bool raycastFace(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3& face_pos, Face* face);
	static void expandToFitFace(const glm::vec3& pos, Face* face, glm::vec3& min, glm::vec3& max);

	typedef BVH<Face>::RaycastResult RaycastResult;
	RaycastResult raycast(const glm::vec3& pos, const glm::vec3& ray);
	RaycastResult last_successful_raycast;
	
	// chunks are initialized with their coordinates, random seed, and unit length
	Chunk(int x, int y, int z, long seed, float unit_length = 1);
	
	// returns the chunk's position in world space, rather than chunk coordinates.
	glm::vec3 getPosf();
	
	// fills a vector array with 4-byte face data structures (using 32-bit ints). Returns the number
	// of faces generated. Requires a Group of the 6 adjacent chunks in order to fill in faces of the blocks on the boundaries.
	// -Also generates a BVH of the resulting face data for raycasting purposes.
	int generateFaceData(std::vector<int>& data, Group neighboring_chunks);
	
	
	// Encodes info about a face into a single 32-bit int. x, y, z are in block coordinates within the chunk
	// (i.e. 0 -> CHUNK_SIZE - 1), face is the normal direction in standard format (0, 1, 2, 3, 4, 5 -> up, down, left, right, forward, back).
	// Color param is a float vector, but is convereted into a reduced-bit version when encoded (3 bits per channel currently).
	int encodeFaceData(int x, int y, int z, int face, glm::vec3 color);
	
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