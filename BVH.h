#pragma once
#ifndef VISIBLE_FACES_BVH
#define VISIBLE_FACES_BVH

#include "Config.h"
#include "util.h"

template <class T>
class BVH
{
public:
	struct RaycastResult
	{
		bool hit;
		T* obj;
		glm::vec3 pos;
	};
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
		bool resized;

		glm::vec3 min {};
		glm::vec3 max {};

		Box(void (*expandToFit)(const glm::vec3&, const T&, glm::vec3&, glm::vec3&));
		~Box();
		
		DataNode* raycast(const glm::vec3& pos, const glm::vec3& ray, bool (*raycastObj)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*));
		void addDataNode(const glm::vec3& pos, const T& obj);
		int countDataNodes();
		//void expandToFit(const glm::vec3& pos);
		void (*expandToFit)(const glm::vec3& pos, const T& obj, glm::vec3& min, glm::vec3& max);

		void split(int min_data_nodes);
	};

	Box* root;
	bool (*raycastObjFunc)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*);
	void (*boxExpandToFitFunc)(const glm::vec3& pos, const T& obj, glm::vec3& min, glm::vec3& max);

	BVH() : raycastObjFunc(nullptr), boxExpandToFitFunc(nullptr) {}
	BVH(
		bool (*raycastObjFunc)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*),
		void (*boxExpandToFitFunc)(const glm::vec3&, const T&, glm::vec3&, glm::vec3&)
	);
	~BVH();

	RaycastResult raycast(const glm::vec3& pos, const glm::vec3& ray);
	void reset();
};

/////////////////////////// TEMPLATE FUNCTION DEFINITIONS //////////////////////////////

template <class T>
BVH<T>::BVH(
	bool (*raycastObjFunc)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*),
	void (*boxExpandToFitFunc)(const glm::vec3&, const T&, glm::vec3&, glm::vec3&)
)	: raycastObjFunc(raycastObjFunc)
	, boxExpandToFitFunc(boxExpandToFitFunc)
{
	root = new Box(boxExpandToFitFunc);
}

template <class T>
BVH<T>::~BVH()
{
	delete root;
}

template <class T>
BVH<T>::Box::Box(void (*expandToFit)(const glm::vec3&, const T&, glm::vec3&, glm::vec3&))
	: expandToFit(expandToFit)
	, data(nullptr), childA(nullptr), childB(nullptr), resized(false)
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

extern int data_node_called;

template <class T>
void BVH<T>::Box::addDataNode(const glm::vec3& pos, const T& obj)
{
	data_node_called++;

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
	resized = true;
	expandToFit(node->pos, node->obj, min, max);
}

//template <class T>
//void BVH<T>::Box::expandToFit(const glm::vec3& pos)
//{
//	resized = true;
//	//std::cout << "expandToFit called with position: " << vec2string(pos);
//	min = glm::min(min, pos);
//	max = glm::max(max, pos + glm::vec3(1));
//	//std::cout << " results min: " << vec2string(min) << ", max: " << vec2string(max) << std::endl;
//}

template <class T>
int BVH<T>::Box::countDataNodes()
{
	int count = 0;
	DataNode* node = data;
	while (node)
	{
		count++;
		node = node->next;
	}
	return count;
}

template <class T>
void BVH<T>::Box::split(int min_data_nodes)
{
	//std::cout << "split called" << std::endl;

	glm::vec3 size = max - min;

	glm::vec3 center = (max + min) * 0.5f;
	int longest_axis = (size.x > max2(size.y, size.z)) ? 0 : (size.y > size.z ? 1 : 2);

	childA = new Box(expandToFit);
	childB = new Box(expandToFit);

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

	if (childA)
	{
		int num_nodes = childA->countDataNodes();
		if (!num_nodes)
		{
			delete childA;
			childA = nullptr;
		}
		else if (num_nodes > min_data_nodes)
			childA->split(min_data_nodes);
	}
	if (childB)
	{
		int num_nodes = childB->countDataNodes();
		if (!num_nodes)
		{
			delete childB;
			childB = nullptr;
		}
		else if (num_nodes > min_data_nodes)
			childB->split(min_data_nodes);
	}

	//std::cout << "return from falloff" << std::endl;
}

template <class T>
bool BVH<T>::Box::hitByRay(const glm::vec3& pos, const glm::vec3& ray)
{
	for (int face = 0; face < 6; face++)
	{
		Quad quad(min, max, face);
		if (rayIntersectsPoly(pos, ray, quad.verts, 4))
			return true;
	}
	return false;
}

template <class T>
auto BVH<T>::Box::raycast(const glm::vec3& pos, const glm::vec3& ray, bool (*raycastObj)(const glm::vec3&, const glm::vec3&, const glm::vec3&, T*))->DataNode*
{
	DataNode* nearest_hit = nullptr;
	if (!resized)
		return nearest_hit;

	if (!this->hitByRay(pos, ray))
	{
		//std::cout << "box missed" << std::endl;
		return nearest_hit;
	}
	std::cout << "box " << this << " hit, size: " << vec2string(max - min) << std::endl;
	
	// find hits from child nodes
	std::vector<DataNode*> hit_nodes;
	DataNode* child_hit = nullptr;
	if (childA && (child_hit = childA->raycast(pos, ray, raycastObj)))
		hit_nodes.push_back(child_hit);
	if (childB && (child_hit = childB->raycast(pos, ray, raycastObj)))
		hit_nodes.push_back(child_hit);	
	
	std::cout << "box " << this << " back from children: " << std::endl;
	std::cout << "objs hit from children: " << hit_nodes.size() << std::endl;
	// find hits from own nodes
	DataNode* node = data;
	while (node)
	{
		if (raycastObj(pos, ray, node->pos, &node->obj))
		{
			hit_nodes.push_back(node);
		}
		node = node->next;
	}
	std::cout << "objs hit after searching own: " << hit_nodes.size() << std::endl;

	// find nearest hit
	if (hit_nodes.size())
	{
		int nearest = 0;
		float nearest_dist = glm::length(hit_nodes[0]->pos - pos);
		for (int i = 1; i < hit_nodes.size(); i++)
		{
			float dist = glm::length(hit_nodes[i]->pos - pos);
			if (dist < nearest_dist)
			{
				nearest = i;
				nearest_dist = dist;
			}
		}
		nearest_hit = hit_nodes[nearest];
	}
	
	return nearest_hit;
}

template <typename T>
auto BVH<T>::raycast(const glm::vec3& pos, const glm::vec3& ray)->RaycastResult
{
	RaycastResult result{};
	if (!root || !raycastObjFunc)
		return result;
	std::cout << "Raycast begin" << std::endl;

	DataNode* hit_node = root->raycast(pos, ray, raycastObjFunc);
	if (!hit_node)
		return result;

	result.hit = true;
	result.obj = &(hit_node->obj);
	result.pos = hit_node->pos;
	return result;
}

template <class T>
void BVH<T>::reset()
{
	delete root;
	root = new Box(boxExpandToFitFunc);
}

#endif