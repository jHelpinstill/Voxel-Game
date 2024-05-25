#pragma once
#ifndef MESH_
#define MESH_

#include "config.h"
#include "Camera.h"
#include "Shader.h"

class Mesh
{
public:
	Camera* camera;
	Shader* shader;

	struct Tri
	{
		glm::vec3 verts[3];
		glm::vec3 norm;
		Tri(glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			verts[0] = a;
			verts[1] = b;
			verts[2] = c;
		}
	};
	std::vector<Tri> tris;

	Mesh() {}
	Mesh(std::vector<Tri>& tris);

	void draw();

	void attachShader(Shader& shader);
	void attachCamera(Camera& camera);

private:
	unsigned int VAO, VBO, vertex_count;

	void setNorm(Tri& tri);
	void createVAO();
};

#endif