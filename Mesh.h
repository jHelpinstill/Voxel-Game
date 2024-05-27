#pragma once
#ifndef MESH_
#define MESH_

#include "config.h"
#include "Camera.h"
#include "Shader.h"
#include "Transform.h"

class Mesh
{
public:
	std::string name;

	Camera* camera;
	Shader* shader;
	Transform transform;
	glm::vec3 color = glm::vec3(0.9, 0.9, 0.0);
	bool use_color = false;

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

	Mesh(const std::string& name, std::vector<Tri>& tris);

	void draw();

	void attachShader(Shader& shader);
	void attachCamera(Camera& camera);
	void updateVAO();

	~Mesh();

	static Mesh* makeBox(const std::string& name, float l, float w, float h, glm::vec3 pos = glm::vec3(0, 0, 0));

private:
	unsigned int VAO, VBO, vertex_count;

	void setNorm(Tri& tri);
	void createVAO();
};

#endif