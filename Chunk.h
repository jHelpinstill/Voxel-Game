#pragma once
#ifndef CHUNK_
#define CHUNK_

#include "config.h"
#include "ObjectManager.h"
#include "BlockType.h"
#include "Mesh.h"
#include "BVH.h"

constexpr auto CHUNK_SIZE = 32;	// number of blocks per side

/*
* The chunk class serves to distinguish groups of blocks from one another in order to reduce the time needed for
* certain operations.
*/
class Chunk
{
public:

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

	// Keys are used to distinguish different chunks within unordered maps by using their coordinates 
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

	/* 
	* x, y, z: the chunk's coordinates in the world in units of CHUNK_SIZE
	* ID: chunk's position in the worlds chunk_draw_params array (used for drawing world fron instanced quad)
	* faces: number of quad instances belonging to this chunk in world mesh
	*/
	BlockType blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]{};	// the main array of blocks
	int x, y, z, ID, faces;
	

	float unit_length;		// length of one block in world coordinates
	long seed;				// unique random number used for various things

	
	/*
	* Bounding Volume Hierarchy used for raycasting. Enables the quick discovery of
	* which face in the world mesh (belonging to this chunk) is intersected by a ray.
	* Works similarly to a binary search, only in 3D space.
	*/
	BVH<int> faces_BVH;
	static bool raycastFace(const glm::vec3& pos, const glm::vec3& ray, const glm::vec3& face_pos, int* face);
	
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

// The following code is used for the hashing of chunks using the custom Chunk::Key, for use in unordered maps of chunks
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