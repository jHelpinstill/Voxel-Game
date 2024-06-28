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
		bool hitByRay(const glm::vec3& pos, const glm::vec3& ray);

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
	std::cout << "box size: " << vec2string(max - min) << std::endl;
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

	// figure out which children to search and, if both, in which order
	Box* ranked_children[2]{};
	bool childA_hit = (childA && childA->hitByRay(pos, ray));
	bool childB_hit = (childB && childB->hitByRay(pos, ray));

	std::cout << "box " << this << " ";
	if (childA_hit && !childB_hit)
	{
		std::cout << "searching childA" << std::endl;
		ranked_children[0] = childA;
	}
	else if (!childA_hit && childB_hit)
	{
		std::cout << "searching childB" << std::endl;
		ranked_children[0] = childB;
	}
	else if (childA_hit && childB_hit)
	{
		std::cout << "searching both children: ";

		float distA, distB;
		distA = glm::length((childA->max - childA->min) - pos);
		distB = glm::length((childB->max - childB->min) - pos);
		if (distA < distB)
		{
			std::cout << "A closer" << std::endl;
			ranked_children[0] = childA;
			ranked_children[1] = childB;
		}
		else
		{
			std::cout << "B closer" << std::endl;
			ranked_children[0] = childB;
			ranked_children[1] = childA;
		}
	}
	else
		std::cout << "has no children" << std::endl;

	// find hits from child nodes
	std::vector<DataNode*> hit_nodes;
	DataNode* hit = nullptr;
	for (int i = 0; i < 2; i++)
	{
		if (ranked_children[i] && (hit = ranked_children[i]->raycast(pos, ray, raycastObj)))
		{
			hit_nodes.push_back(hit);
			break;
		}
	}

	std::cout << "box " << this << " back from children: " << std::endl;
	std::cout << "hits received from children: " << hit_nodes.size() << std::endl;
	int hits_before_self = hit_nodes.size();
	// find hits from own nodes
	DataNode* node = data;
	while (node)
	{
		if (raycastObj(pos, ray, node->pos, &node->obj))
		{
			hit_nodes.push_back(node);
			std::cout << "hit face with direction: " << node->obj << std::endl;
		}
		node = node->next;
	}
	std::cout << "hits from own data: " << hit_nodes.size() - hits_before_self << std::endl;

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

	DataNode* hit_node = nullptr;
	if (root->hitByRay(pos, ray))
		hit_node = root->raycast(pos, ray, raycastObjFunc);
	else
		std::cout << "root box missed" << std::endl;
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