#pragma once
#ifndef BLOCK_TYPE
#define BLOCK_TYPE

#include "config.h"

enum class BlockType
{
	AIR,
	DIRT
};

std::string inline getBlockName(BlockType b)
{
	switch (b)
	{
	case BlockType::AIR: return "AIR";
	case BlockType::DIRT: return "DIRT";
	}
	return "";
}

#endif