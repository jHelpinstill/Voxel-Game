#pragma once
#ifndef BLOCK_TYPE
#define BLOCK_TYPE

#include "config.h"

enum class BlockType
{
	AIR,
	DIRT,
	STONE
};

inline std::string getBlockName(BlockType b)
{
	switch (b)
	{
	case BlockType::AIR: return "AIR";
	case BlockType::DIRT: return "DIRT";
	case BlockType::STONE: return "STONE";
	}
	return "";
}

inline float uDist(int rand_num)
{
	return (rand_num != RAND_MAX) ? (rand_num / (float)(RAND_MAX)) : (--rand_num / (float)(RAND_MAX));
}

inline float colorRange(float lower, float higher, int rand_num, float quantize = 0)
{
	float t = uDist(rand_num) * (higher - lower);
	float color = lower + t;
	if (quantize > 0)
	{
		color *= quantize;
		color = floor(color);
		color /= quantize;
	}
	return color;
}

inline glm::vec3 getBlockColor(BlockType block, int face, int rand_num = -1)
{
	if(rand_num == -1)
		rand_num = std::rand();
	switch (block)
	{
	case BlockType::DIRT:
	{
		switch(face)
		{
		case 0:
			return glm::vec3(0, colorRange((9 / 16.0f), (14 / 16.0f), rand_num, 16), 0);
			//return glm::vec3(0, (float)((rand_num % 3) + 4) / 7.0f, 0);
		default:
			return glm::vec3(0.5, 0.4, 0);
		}
	}
	case BlockType::STONE:
	{
		//rand_num = (rand_num << 1) ^ rand_num;
		rand_num = rand_num / (RAND_MAX / 3);
		if (rand_num > 1)
			rand_num = 1;
		float color = (float)(rand_num + 4) / 7.0f;
		return glm::vec3(color, color, color);
	}
	}
	return glm::vec3(1, 0, 1);
}

#endif