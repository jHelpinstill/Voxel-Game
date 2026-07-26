#include "BlockType.h"
#include <random>

std::string getBlockName(BlockType b) {
	switch (b) {
	case BlockType::AIR: return "AIR";
	case BlockType::DIRT: return "DIRT";
	case BlockType::STONE: return "STONE";
	}
	return "";
}

float uDist(int rand_num) {
	return (rand_num != RAND_MAX) ? (rand_num / (float)(RAND_MAX)) : (--rand_num / (float)(RAND_MAX));
}

int randFromList(std::vector<int> choices, int rand_num) {
	int index = choices.empty() ? 0 : floor(uDist(rand_num) * choices.size());
	return choices[index];
}

float colorRange(int lower, int higher, int rand_num) {
	return floor(uDist(rand_num) * (higher + 1 - lower));
}

std::uniform_int_distribution<int> new_rand_dist(0, RAND_MAX);
glm::ivec3 getBlockColor(BlockType block, int face, int rand_num) {
	glm::ivec3 color_ret = {-1, 0, -1};
	if(rand_num == -1)
		rand_num = std::rand();
    std::minstd_rand engine(rand_num);
    for(int i = 0; i < 100; i++)
        new_rand_dist(engine);
	switch (block) {
		case BlockType::DIRT: { // a comment 
			switch(face) {
				case 0: {
					bool flower = (uDist(rand_num) < (1.0 / 1000));
					int color;
					if(flower) {
						glm::ivec3 colors[] = {
							{14, 0, 0},
							{0, 0, 14},
							{14, 0, 14},
							{14, 14, 0}
						};
						color_ret = colors[randFromList({0, 1, 2, 3}, new_rand_dist(engine))];
					}
					else
						color_ret = glm::ivec3(0, randFromList({8, 9, 10, 10, 10, 10, 10, 11, 12}, rand_num), 0);
				} break;

				default: {
					color_ret = glm::ivec3(
						randFromList({4, 4, 6}, rand_num),
						randFromList({2, 4}, rand_num),
						0
					);
				} break;
			}
		} break;

		case BlockType::STONE: {
			int c = randFromList({8, 10, 10, 10, 10, 10, 10, 12}, rand_num);
			color_ret = glm::ivec3(c, c, c);

			// Clown Vomit //
			// int rand_g = new_rand_dist(engine);
			// int rand_b = new_rand_dist(engine);
			// color_ret = glm::vec3(
            //     randFromList({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, rand_num), 
            //     randFromList({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, rand_g), 
            //     randFromList({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, rand_b)
            // );
		} break;
	}
	return color_ret;
}