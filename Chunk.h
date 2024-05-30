#pragma once
#ifndef CHUNK_
#define CHUNK_

#include "config.h"
#include "BlockType.h"

#define CHUNK_SIZE 32

class Chunk
{
public:
	BlockType blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];


	Chunk() {}
};

#endif