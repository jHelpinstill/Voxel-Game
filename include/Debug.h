#pragma once
#ifndef DEBUG_
#define DEBUG_

#include "Config.h"
#include "BVH.h"
#include "Chunk.h"
#include "ChunkManager.h"

void putMeshWhereLooking(ChunkManager::RaycastResult cast, const std::string& mesh_name);

void printBlockInfo(BlockType* block, Chunk* chunk);

struct MonitorBVHface
{
	int boxes_without_data;
	int num_boxes;
	int num_leaf_boxes;
	int num_with_single_child;
	int num_with_both_children;

	int max_data_nodes;
	float avg_data_nodes;
	int sum_data_nodes;

	int num_faces[6];
};

void traceBVHface(BVH<Chunk::Face>& bvh);
void traceBVHface(BVH<Chunk::Face>::Box* box, MonitorBVHface& monitor);

struct MonitorBVHchunk
{
	int boxes_without_data;
	int num_boxes;
	int num_leaf_boxes;
	int num_with_single_child;
	int num_with_both_children;

	int max_data_nodes;
	float avg_data_nodes;
	int sum_data_nodes;
};

void traceBVHchunk(BVH<Chunk*>& bvh);
void traceBVHchunk(BVH<Chunk*>::Box* box, MonitorBVHchunk& monitor);

#endif