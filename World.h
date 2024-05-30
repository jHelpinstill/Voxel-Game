#pragma once
#ifndef WORLD_
#define WORLD_

#include "config.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "Chunk.h"

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

template <>
struct std::hash<Key>
{
	std::size_t operator()(const Key& k) const
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

class World
{
private:

public:
	float chunk_unit_dimension = 1.0;
	int chunk_radius = 4;

	std::unordered_map<Key, Chunk*> chunks;

	World() {}

	void setup();
	void generateMesh();
};

#endif