#pragma once
#include "config.h"

class TriangleMesh
{
private:
	unsigned int VBO, VAO, vertex_count;

public:
	TriangleMesh();

	void draw();
	~TriangleMesh();
};