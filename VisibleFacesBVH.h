#pragma once
#ifndef VISIBLE_FACES_BVH
#define VISIBLE_FACES_BVH

#include "Config.h"
#include "util.h"
#include "BlockType.h"
#include "Chunk.h"

class VisibleFacesBVH
{
private:
	struct FaceData
	{
		glm::vec3 pos;
		BlockType* block;
		Chunk* chunk;
		FaceData* next;
	};
	class Box
	{
	public:
		FaceData* faces;
		Box* childA;
		Box* childB;

		glm::vec3 min;
		glm::vec3 max;

		Box();

		bool hitByRay(const glm::vec3& pos, const glm::vec3& ray);

		void addFace(const glm::vec3& pos, BlockType* block, Chunk* chunk);
		void addFace(FaceData* face);
		void expandToFit(const glm::vec3& pos);

		void split();
	};

public:
	Box* root;

	VisibleFacesBVH() : root(nullptr) {}

	bool raycast(const glm::vec3& pos, const glm::vec3& ray, BlockType** block, Chunk** chunk);
};

#endif