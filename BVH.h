#pragma once
#ifndef VISIBLE_FACES_BVH
#define VISIBLE_FACES_BVH

#include "Config.h"
#include "util.h"
#include "BlockType.h"
#include "Chunk.h"

template <class T>
class VisibleFacesBVH
{
private:
	struct DataNode
	{
		glm::vec3 pos;
		T* obj;
		DataNode* next;
	};
	class Box
	{
	public:
		DataNode* data;
		Box* childA;
		Box* childB;

		glm::vec3 min;
		glm::vec3 max;

		Box();

		bool hitByRay(const glm::vec3& pos, const glm::vec3& ray);

		void addDataNode(const glm::vec3& pos, T* obj);
		void addDataNode(DataNode* face);
		void expandToFit(const glm::vec3& pos);

		void split();
	};

public:
	Box* root;

	VisibleFacesBVH() : root(nullptr) {}

	bool raycast(const glm::vec3& pos, const glm::vec3& ray, T** obj);
};

#endif