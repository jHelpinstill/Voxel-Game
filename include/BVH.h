#pragma once
#ifndef VISIBLE_FACES_BVH
#define VISIBLE_FACES_BVH

#include "Config.h"
#include "util.h"
#include <typeinfo>

template <class T>
class BVH {
public:
	struct DataNode {
		glm::vec3 pos;
		T obj;
		DataNode *next;
	};

	typedef void (*ObjExpansionFunc)(const glm::vec3 &pos, T *obj, glm::vec3 &min, glm::vec3 &max);
	ObjExpansionFunc objExpansionFunc;

	class Box {
	public:
		DataNode *data;
		Box *childA;
		Box *childB;
		bool resized;
		int depth = 0;

		glm::vec3 min {};
		glm::vec3 max {};

		ObjExpansionFunc expandToFit;

		Box(ObjExpansionFunc expandToFit);
		~Box();
		
		void raycast(const glm::vec3 &pos, const glm::vec3 &ray, std::vector<DataNode*> &hits);
		void split(int min_data_nodes);

		void createDataNode(const glm::vec3 &pos, const T &obj);
		void addDataNode(DataNode *node);
		int countDataNodes();

		bool hitByRay(const glm::vec3 &pos, const glm::vec3 &ray);
		bool intersectsSphere(const glm::vec3 &pos, float radius);

		// WARNING: renders tree unusable until BVH::rebuild() is called
		DataNode *getData(DataNode *existing_data = nullptr);
	};
	int min_nodes_per_box;

	Box *root;

	BVH() : root(nullptr), objExpansionFunc(nullptr) {}
	BVH(
		ObjExpansionFunc objExpansionFunc,
		int min_nodes = 1
	);
	~BVH();

	void raycast(const glm::vec3 &pos, const glm::vec3 &ray, std::vector<DataNode*> &hits);
	void reset();
	void rebuild();
	void build();
};

/////////////////////////// BVH FUNCTION DEFINITIONS //////////////////////////////

template <class T>
BVH<T>::BVH(
	ObjExpansionFunc objExpansionFunc,
	int min_nodes
)	: objExpansionFunc(objExpansionFunc)
	, min_nodes_per_box(min_nodes) {
	root = new Box(objExpansionFunc);
}

template <class T>
BVH<T>::~BVH() {
	delete root;
}

template <typename T>
void BVH<T>::raycast(const glm::vec3 &pos, const glm::vec3 &ray, std::vector<DataNode*> &hits) {
	if (root && root->hitByRay(pos, ray))
		root->raycast(pos, ray, hits);
}

template <class T>
void BVH<T>::reset() {
	delete root;
	root = new Box(objExpansionFunc);
}

template <class T>
void BVH<T>::rebuild() {
	root->data = root->getData();

	delete root->childA;
	root->childA = nullptr;
	delete root->childB;
	root->childB = nullptr;

	root->split(min_nodes_per_box);
}

template <class T>
void BVH<T>::build() {
	root->split(min_nodes_per_box);
}

/////////////////////////// BOX FUNCTION DEFINITIONS //////////////////////////////

template <class T>
BVH<T>::Box::Box(ObjExpansionFunc expandToFit)
	: expandToFit(expandToFit)
	, data(nullptr), childA(nullptr), childB(nullptr), resized(false) {
	min = glm::vec3(std::numeric_limits<float>::infinity());
	max = glm::vec3(-std::numeric_limits<float>::infinity());
}

template <class T>
BVH<T>::Box::~Box() {
	delete childA;
	delete childB;

	while (data) {
		DataNode *old = data;
		data = data->next;
		delete old;
	}
}

template <class T>
void BVH<T>::Box::createDataNode(const glm::vec3 &pos, const T &obj) {
	DataNode *node = new DataNode;
	node->pos = pos;
	node->obj = obj;

	addDataNode(node);
}

template <class T>
void BVH<T>::Box::addDataNode(DataNode *node) {
	node->next = data;
	data = node;
	resized = true;
	expandToFit(node->pos, &node->obj, min, max);
}

template <class T>
int BVH<T>::Box::countDataNodes() {
	int count = 0;
	DataNode *node = data;
	while (node) {
		count++;
		node = node->next;
	}
	return count;
}

template <class T>
auto BVH<T>::Box::getData(DataNode *existing_data)->DataNode* {
	if (childA)
		existing_data = childA->getData(existing_data);
	if (childB)
		existing_data = childB->getData(existing_data);

	if (!data)
		return existing_data;

	DataNode *tail = data;
	while (tail->next)
		tail = tail->next;
	tail->next = existing_data;

	existing_data = data;
	data = nullptr;
	return existing_data;
}

class Chunk;

template <class T>
void BVH<T>::Box::split(int min_data_nodes) {
	static int split_depth = 0;
	split_depth++;

	if (typeid(T) == typeid(Chunk*)) {
		
		if (split_depth > 20) {
			std::cout << "max depth exceeded: splits: " << split_depth << std::endl;
		}
	}

	glm::vec3 size = max - min;

	glm::vec3 center = (max + min) * 0.5f;
	int longest_axis = (size.x > max2(size.y, size.z)) ? 0 : (size.y > size.z ? 1 : 2);

	if(split_depth > 20)
		std::cout << "size: " << vec2string(size) << "--" << "center: " << vec2string(center) << std::endl;

	childA = new Box(expandToFit);
	childB = new Box(expandToFit);

	childA->depth = childB->depth = split_depth;

	float splittingPoint = center[longest_axis];
	DataNode *node = data;
	while (node) {
		DataNode *current_node = node;
		node = node->next;

		if (current_node->pos[longest_axis] < splittingPoint) {
			childA->addDataNode(current_node);
		}
		else {
			childB->addDataNode(current_node);
		}
	}

	data = nullptr;

	if (childA) {
		int num_nodes = childA->countDataNodes();
		if (!num_nodes) {
			delete childA;
			childA = nullptr;
		}
		else if (num_nodes > min_data_nodes)
			childA->split(min_data_nodes);
	}
	if (childB) {
		int num_nodes = childB->countDataNodes();
		if (!num_nodes) {
			delete childB;
			childB = nullptr;
		}
		else if (num_nodes > min_data_nodes)
			childB->split(min_data_nodes);
	}

	split_depth--;
}

template <class T>
bool BVH<T>::Box::hitByRay(const glm::vec3 &pos, const glm::vec3 &ray) {
	for (int face = 0; face < 6; face++) {
		Quad quad(min, max, face);
		if (rayIntersectsPoly(pos, ray, quad.verts, 4))
			return true;
	}
	return false;
}

template <class T>
bool BVH<T>::Box::intersectsSphere(const glm::vec3 &pos, float radius) {
	for(int face = 0; face < 6; face++) {
		Quad quad(min, max, face);
		glm::vec3 norm = getPolyNorm(quad.verts, 4, util::PolyCulling::CCW);
	}
	return true;
}

template <class T>
void BVH<T>::Box::raycast(const glm::vec3 &pos, const glm::vec3 &ray, std::vector<DataNode*> &hits) {
	if (!resized)
		return;	// don't bother if box hasn't been resized yet (not initialized with data)

	for(DataNode *node = data; node != nullptr; node = node->next)
		hits.push_back(node);

	bool childA_hit = (childA && childA->hitByRay(pos, ray));
	bool childB_hit = (childB && childB->hitByRay(pos, ray));
	if(childA_hit)
		childA->raycast(pos, ray, hits);
	if(childB_hit)
		childB->raycast(pos, ray, hits);
}

#endif