#pragma once
#include "config.h"

class TriangleMesh
{
private:
	unsigned int VBO, VAO, vertex_count;
	void createVAO(float* verts, float* colors);

public:
	TriangleMesh();
	TriangleMesh(float* verts, float* colors);

	void draw();
	~TriangleMesh();
};