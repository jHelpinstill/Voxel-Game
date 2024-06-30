#include "Debug.h"


void traceBVHi(BVH<int>& bvh)
{
	MonitorBVHi monitor{};

	traceBVHi(bvh.root, monitor);

	if(monitor.sum_data_nodes)
		monitor.avg_data_nodes = monitor.sum_data_nodes / (float)(monitor.num_boxes - monitor.boxes_without_data);

	std::cout << "BVH MONITOR RESULTS" << std::endl;
	std::cout << "boxes: " << monitor.num_boxes << std::endl;
	std::cout << "-without data: " << monitor.boxes_without_data << std::endl;
	std::cout << "-leaves: " << monitor.num_leaf_boxes << std::endl;
	std::cout << "-with one child node: " << monitor.num_with_single_child << std::endl;
	std::cout << "-with both child nodes: " << monitor.num_with_both_children << std::endl;
	std::cout << "most data nodes: " << monitor.max_data_nodes << std::endl;
	std::cout << "avg data nodes: " << monitor.avg_data_nodes << std::endl;

	for (int i = 0; i < 6; i++)
	{
		std::cout << "num faces in direction " << i << ": " << monitor.num_faces[i] << std::endl;
	}
}

void traceBVHi(BVH<int>::Box* box, MonitorBVHi& monitor)
{
	monitor.num_boxes++;
	if (!box->data)
		monitor.boxes_without_data++;
	if (!box->childA && !box->childB)
	{
		monitor.num_leaf_boxes++;

		int num_data_nodes = 0;
		BVH<int>::DataNode* node = box->data;
		while (node)
		{
			monitor.num_faces[node->obj]++;
			num_data_nodes++;
			node = node->next;
		}

		monitor.sum_data_nodes += num_data_nodes;
		if (num_data_nodes > monitor.max_data_nodes)
			monitor.max_data_nodes = num_data_nodes;
	}
	else if (box->childA && box->childB)
	{
		monitor.num_with_both_children++;
		traceBVHi(box->childA, monitor);
		traceBVHi(box->childB, monitor);
	}
	else if (box->childA)
	{
		monitor.num_with_single_child++;
		traceBVHi(box->childA, monitor);
	}
	else
	{
		monitor.num_with_single_child++;
		traceBVHi(box->childB, monitor);
	}
}

void traceBVHchunk(BVH<Chunk*>& bvh)
{
	MonitorBVHchunk monitor{};

	traceBVHchunk(bvh.root, monitor);

	if (monitor.sum_data_nodes)
		monitor.avg_data_nodes = monitor.sum_data_nodes / (float)(monitor.num_boxes - monitor.boxes_without_data);

	std::cout << "BVH<Chunk*> MONITOR RESULTS" << std::endl;
	std::cout << "boxes: " << monitor.num_boxes << std::endl;
	std::cout << "-without data: " << monitor.boxes_without_data << std::endl;
	std::cout << "-leaves: " << monitor.num_leaf_boxes << std::endl;
	std::cout << "-with one child node: " << monitor.num_with_single_child << std::endl;
	std::cout << "-with both child nodes: " << monitor.num_with_both_children << std::endl;
	std::cout << "most data nodes: " << monitor.max_data_nodes << std::endl;
	std::cout << "avg data nodes: " << monitor.avg_data_nodes << "\n" << std::endl;
}

void traceBVHchunk(BVH<Chunk*>::Box* box, MonitorBVHchunk& monitor)
{
	monitor.num_boxes++;
	if (!box->data)
		monitor.boxes_without_data++;
	if (!box->childA && !box->childB)
	{
		monitor.num_leaf_boxes++;

		int num_data_nodes = 0;
		BVH<Chunk*>::DataNode* node = box->data;
		while (node)
		{
			num_data_nodes++;
			node = node->next;
		}

		monitor.sum_data_nodes += num_data_nodes;
		if (num_data_nodes > monitor.max_data_nodes)
			monitor.max_data_nodes = num_data_nodes;
	}
	else if (box->childA && box->childB)
	{
		monitor.num_with_both_children++;
		traceBVHchunk(box->childA, monitor);
		traceBVHchunk(box->childB, monitor);
	}
	else if (box->childA)
	{
		monitor.num_with_single_child++;
		traceBVHchunk(box->childA, monitor);
	}
	else
	{
		monitor.num_with_single_child++;
		traceBVHchunk(box->childB, monitor);
	}
}