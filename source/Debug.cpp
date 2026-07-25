#include "Debug.h"

void putMeshWhereLooking(ChunkManager::RaycastResult cast, const std::string& mesh_name)
{
	if (cast.hit)
	{
		Mesh* test_block = getMeshByName(mesh_name);
		float length = cast.chunk->unit_length;
		static glm::vec3 offset[6] =
		{
			glm::vec3(0, length, 0),
			glm::vec3(0, -length, 0),
			glm::vec3(length, 0, 0),
			glm::vec3(-length, 0, 0),
			glm::vec3(0, 0, length),
			glm::vec3(0, 0, -length)
		};
		test_block->transform.pos = cast.pos + offset[cast.face];
	}
}

void printBlockInfo(BlockType* block, Chunk* chunk)
{
	std::cout << "block data:  " << std::endl;
	std::cout << "\ttype:      " << getBlockName(*block) << std::endl;
	int x, y, z; chunk->blocks.getCoords(block, x, y, z);
	std::cout << "\tgrid pos:  " << x << ", " << y << ", " << z << std::endl;
	std::cout << "\tworld pos: " << vec2string(chunk->getPosf() + glm::vec3(x, y, z) * chunk->unit_length) << std::endl;
	std::cout << "\tindex:     " << chunk->blocks.getIndex(block) << std::endl;
	std::cout << "\tpointer:   " << block << std::endl;
	std::cout << std::endl;
}

void traceBVHface(BVH<Chunk::Face>& bvh)
{
	MonitorBVHface monitor{};

	traceBVHface(bvh.root, monitor);

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
	std::cout << std::endl;
}

void traceBVHface(BVH<Chunk::Face>::Box* box, MonitorBVHface& monitor)
{
	monitor.num_boxes++;
	if (!box->data)
		monitor.boxes_without_data++;
	if (!box->childA && !box->childB)
	{
		monitor.num_leaf_boxes++;

		int num_data_nodes = 0;
		BVH<Chunk::Face>::DataNode* node = box->data;
		while (node)
		{
			monitor.num_faces[node->obj.norm]++;
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
		traceBVHface(box->childA, monitor);
		traceBVHface(box->childB, monitor);
	}
	else if (box->childA)
	{
		monitor.num_with_single_child++;
		traceBVHface(box->childA, monitor);
	}
	else
	{
		monitor.num_with_single_child++;
		traceBVHface(box->childB, monitor);
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