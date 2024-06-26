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
		bool hitByRay(const glm::vec3& pos, const glm::vec3& ray);

	public:
		DataNode* data;
		Box* childA;
		Box* childB;

		glm::vec3 min;
		glm::vec3 max;

		Box();
		~Box();

		
		bool raycast(const glm::vec3& pos, const glm::vec3& ray, T** obj_out, glm::vec3* obj_pos, bool (*raycastObj)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*));
		void addDataNode(const glm::vec3& pos, const T& obj);
		void expandToFit(const glm::vec3& pos);

		void split();
	};

	Box* root;
	bool (*raycastObj)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*);

	BVH();
	~BVH();

	bool raycast(const glm::vec3& pos, const glm::vec3& ray, T** obj_out, glm::vec3* obj_pos);
	void reset();
};

/////////////////////////// TEMPLATE FUNCTION DEFINITIONS //////////////////////////////

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
	glm::vec3 size = max - min;
	if ((int)size.x == 1 && (int)size.y == 1 && (int)size.z == 1)
		return;	// dont split if size is 1x1x1

	glm::vec3 center = (max + min) * 0.5f;
	int longest_axis = (size.x > max2(size.y, size.z)) ? 0 : (size.y > size.z ? 1 : 2);

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
		childA->split();
	if (childB)
		childB->split();
}

template <class T>
bool BVH<T>::Box::hitByRay(const glm::vec3& pos, const glm::vec3& ray)
{
	Quad quad;
	for (int face = 0; face < 6; face++)
	{
		quad = Quad(min, max, face);
		if (rayIntersectsPoly(pos, ray, quad.verts, 4))
			return true;
	}

	return false;
}

template <class T>
bool BVH<T>::Box::raycast(const glm::vec3& pos, const glm::vec3& ray, T** obj_out, glm::vec3* obj_pos, bool (*raycastObj)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*))
{
	if (!this->hitByRay(pos, ray))
		return false;

	DataNode* node = data;
	while (node)
	{
		if (raycastObj(pos, ray, node->pos, &node->obj))
		{
			*obj_out = &node->obj;
			obj_pos = &node->pos;
			return true;
		}
	}

	bool hit = false;
	if (childA->raycast(pos, ray, obj_out, obj_pos, raycastObj))
		hit = true;
	if (childB->raycast(pos, ray, obj_out, obj_pos, raycastObj))
		hit = true;
	return hit;
}

template <class T>
bool BVH<T>::raycast(const glm::vec3& pos, const glm::vec3& ray, T** obj_out, glm::vec3* obj_pos)
{
	if (!root || !raycastObj)
		return false;

	return root->raycast(pos, ray, obj_out, obj_pos, raycastObj);
}

template <class T>
void BVH<T>::reset()
{
	delete root;
	root = new Box;
}

#endif