#pragma once
#ifndef BLOCK_TYPE
#define BLOCK_TYPE

#include "config.h"

enum class BlockType {
	AIR,
	DIRT,
	STONE
};

std::string getBlockName(BlockType b);
float uDist(int rand_num);
int randFromList(std::vector<int> choices, int rand_num);
float colorRange(int lower, int higher, int rand_num);
glm::ivec3 getBlockColor(BlockType block, int face, int rand_num = -1);

#endif