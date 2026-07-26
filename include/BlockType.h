#pragma once
#ifndef BLOCK_TYPE
#define BLOCK_TYPE

#include "config.h"

enum class BlockType {
	AIR,
	DIRT,
	STONE
};

inline std::string getBlockName(BlockType b) {
	switch (b) {
	case BlockType::AIR: return "AIR";
	case BlockType::DIRT: return "DIRT";
	case BlockType::STONE: return "STONE";
	}
	return "";
}

inline int newRand(int rand_num) {
	unsigned int a = rand_num;
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	a &= ~(1 << 31);
	a %= RAND_MAX;
	return (int)a;
}

inline float uDist(int rand_num) {
	return (rand_num != RAND_MAX) ? (rand_num / (float)(RAND_MAX)) : (--rand_num / (float)(RAND_MAX));
}

inline int randFromList(std::vector<int> choices, int rand_num) {
	int index = choices.empty() ? 0 : floor(uDist(rand_num) * choices.size());
	return choices[index];
}

inline float colorRange(int lower, int higher, int rand_num) {
	return floor(uDist(rand_num) * (higher + 1 - lower));
}

// inline float colorRange(float lower, float higher, int rand_num) {
// 	float color;
// 	if(lower >= higher)
// 		color = lower;
// 	else {
// 		float range = higher - lower;
// 		float t = uDist(rand_num) * range;
// 		color = lower + t;
// 	}
// 	return color;
// }

// inline float colorRangeQuantized(float lower, float higher, int rand_num, int q) {
// 	float color = colorRange(lower, higher, rand_num);
// 	float t = color - lower;
// 	float range = higher - lower;
// 	if(q > 1) {
// 		float step1 = range / (q - 1);
// 		float step = (range + step1) / q;
// 		t *= (range + step1) / range;
// 		t /= step;
// 		t = floor(t);
// 		t *= step;
// 	}
// 	color = lower + t;
// 	if(color > higher)
// 		color = higher;
// 	return color;
// }

inline glm::ivec3 getBlockColor(BlockType block, int face, int rand_num = -1) {
	if(rand_num == -1)
		rand_num = std::rand();
	switch (block) {
		case BlockType::DIRT: { // a comment 
			switch(face) {
			case 0: {
				int color = randFromList({4, 5, 5, 6}, rand_num);
				return glm::ivec3(0, color, 0);
				
			}
			default: {
				return glm::ivec3(
					randFromList({2, 2, 3}, rand_num),
					randFromList({1, 2}, rand_num),
					0
				);
				// int color = 5; //randFromList({5}, rand_num);
				// return glm::ivec3(color, color, 0);
			}
		}
	}
		case BlockType::STONE: {
			int color = randFromList({4, 5, 5, 6, 6}, rand_num);
			return glm::ivec3(color, color, color);
			// rand_num = rand_num / (RAND_MAX / 3);
			// if (rand_num > 1)
			// 	rand_num = 1;
			// float color = (float)(rand_num + 4) / 7.0f;
			// return glm::vec3(color, color, color);

			// Clown Vomit //
			//int rand_g = newRand(rand_num);
			//int rand_b = newRand(rand_g);
			//return glm::vec3(uDist(rand_num), uDist(rand_g), uDist(rand_b));
		}
	}
	return glm::vec3(1, 0, 1);
}

#endif