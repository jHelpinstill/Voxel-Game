#pragma once
#ifndef DEBUG_
#define DEBUG_

#include "Config.h"
#include "BVH.h"

struct MonitorBVHi
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

void traceBVHi(BVH<int>& bvh);
void traceBVHi(BVH<int>::Box* box, MonitorBVHi& monitor);

#endif