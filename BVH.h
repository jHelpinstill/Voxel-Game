#pragma once
#ifndef VISIBLE_FACES_BVH
#define VISIBLE_FACES_BVH

#include "Config.h"
#include "util.h"

template <class T>
class BVH
{
public:
	struct DataNode
	{
		glm::vec3 pos;
		T obj;
		DataNode* next;
	};
	class Box
	{
	private:
		void addDataNode(DataNode* node);

	public:
		DataNode* data;
		Box* childA;
		Box* childB;

		glm::vec3 min;
		glm::vec3 max;

		Box();
		~Box();

		bool hitByRay(const glm::vec3& pos, const glm::vec3& ray);

		void addDataNode(const glm::vec3& pos, const T& obj);
		void expandToFit(const glm::vec3& pos);

		void split();
	};

	Box* root;

	BVH();
	~BVH();

	bool raycast(const glm::vec3& pos, const glm::vec3& ray, T* obj);
	void reset();
};

template <class T>
BVH<T>::BVH()
{
	root = new Box;
}

template <class T>
BVH<T>::~BVH()
{
	delete root;
}

template <class T>
BVH<T>::Box::Box() : data(nullptr), childA(nullptr), childB(nullptr)
{
	min = glm::vec3(std::numeric_limits<float>::infinity());
	max = glm::vec3(-std::numeric_limits<float>::infinity());
}

template <class T>
BVH<T>::Box::~Box()
{
	delete childA;
	delete childB;

	while (data)
	{
		DataNode* old = data;
		data = data->next;
		delete old;
	}
}


template <class T>
void BVH<T>::Box::addDataNode(const glm::vec3& pos, const T& obj)
{
	DataNode* node = new DataNode;
	node->pos = pos;
	node->obj = obj;

	addDataNode(node);
}

template <class T>
void BVH<T>::Box::addDataNode(DataNode* node)
{
	node->next = data;
	data = node;
	expandToFit(node->pos);
}

template <class T>
void BVH<T>::Box::expandToFit(const glm::vec3& pos)
{
	min = glm::min(min, pos);
	max = glm::max(max, pos + glm::vec3(1));
}

template <class T>
void BVH<T>::Box::split()
{
	//static int rec_depth = 0;
	glm::vec3 size = max - min;
	if ((int)size.x == 1 && (int)size.y == 1 && (int)size.z == 1)
		return;	// dont split if size is 1x1x1

	//rec_depth++;
	//std::cout << rec_depth << std::endl;

	glm::vec3 center = (max + min) * 0.5f;
	int longest_axis = (size.x > max2(size.y, size.z)) ? 0 : (size.y > size.z ? 1 : 2);

	//std::cout << longest_axis << std::endl;

	childA = new Box();
	childB = new Box();

	float splittingPoint = center[longest_axis];
	DataNode* node = data;
	while (node)
	{
		DataNode* current_node = node;
		node = node->next;

		if (current_node->pos[longest_axis] < splittingPoint)
			childA->addDataNode(current_node);
		else
			childB->addDataNode(current_node);
	}

	data = nullptr;

	if (!childA->data)
	{
		delete childA;
		childA = nullptr;
	}
	if (!childB->data)
	{
		delete childB;
		childB = nullptr;
	}

	if (childA)
	{
		//std::cout << "entering childA split: size = " << vec2string(size) << std::endl;
		childA->split();
		//std::cout << "returning from childA split" << std::endl;
	}
	if (childB)
	{
		//std::cout << "entering childB split" << std::endl;
		childB->split();
		//std::cout << "returning from childB split" << std::endl;
	}
	
	//rec_depth--;
}

template <class T>
bool BVH<T>::Box::hitByRay(const glm::vec3& pos, const glm::vec3& ray)
{
	glm::vec3 quad_verts[4];
	glm::vec3 box_verts[8];
	glm::vec3 size = max - min;

	using namespace util;

	box_verts[0] = min;
	box_verts[1] = min + glm::vec3(size.x, 0, 0);
	box_verts[2] = min + glm::vec3(0, size.y, 0);
	box_verts[3] = min + glm::vec3(0, 0, size.z);
	box_verts[4] = min + glm::vec3(size.x, size.y, 0);
	box_verts[5] = min + glm::vec3(0, size.y, size.z);
	box_verts[6] = min + glm::vec3(size.x, 0, size.z);
	box_verts[7] = max;

	// top
	quad_verts[0] = box_verts[2];
	quad_verts[1] = box_verts[5];
	quad_verts[2] = box_verts[7];
	quad_verts[3] = box_verts[4];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// bottom
	quad_verts[0] = box_verts[0];
	quad_verts[1] = box_verts[1];
	quad_verts[2] = box_verts[6];
	quad_verts[3] = box_verts[3];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// left
	quad_verts[0] = box_verts[1];
	quad_verts[1] = box_verts[4];
	quad_verts[2] = box_verts[7];
	quad_verts[3] = box_verts[6];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// right
	quad_verts[0] = box_verts[0];
	quad_verts[1] = box_verts[3];
	quad_verts[2] = box_verts[5];
	quad_verts[3] = box_verts[2];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// forward
	quad_verts[0] = box_verts[3];
	quad_verts[1] = box_verts[6];
	quad_verts[2] = box_verts[7];
	quad_verts[3] = box_verts[5];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	// back
	quad_verts[0] = box_verts[0];
	quad_verts[1] = box_verts[1];
	quad_verts[2] = box_verts[4];
	quad_verts[3] = box_verts[2];
	if (rayIntersectsPoly(pos, ray, quad_verts, 4))
		return true;

	return false;
}

template <class T>
bool BVH<T>::raycast(const glm::vec3& pos, const glm::vec3& ray, T* obj)
{
	if (!root)
		return false;

	Box* current_box = &root;

	while (current_box->childA || current_box->childB)
	{
		if (!current_box->childB)
		{
			current_box = current_box->childA;
			continue;
		}
		if (!current_box->childA)
		{
			current_box = current_box->childB;
			continue;
		}

		bool hitA = current_box->childA->hitByRay(pos, ray);
		bool hitB = current_box->childA->hitByRay(pos, ray);

		if (hitA && !hitB)
		{
			current_box = current_box->childA;
			continue;
		}
		if (hitB && !hitA)
		{
			current_box = current_box->childB;
			continue;
		}
		if (!hitA && !hitB)
			return false;

		// only remaining option is ray intersects both A and B

	}
}

template <class T>
void BVH<T>::reset()
{
	delete root;
	root = new Box;
}

#endif