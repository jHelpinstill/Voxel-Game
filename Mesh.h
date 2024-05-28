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
		float verts[3][3];
		float uv_coords[3][2];

		Tri(glm::vec3 a, glm::vec3 b, glm::vec3 c)
		{
			for (int i = 0; i < 3; i++)
			{
				verts[0][i] = a[i];
				verts[1][i] = b[i];
				verts[2][i] = c[i];
			}
		}
	};
	std::vector<Tri> tris;

	Mesh(const std::string& name, std::vector<Tri>& tris);

	void draw(unsigned int texture);

	void attachShader(Shader& shader);
	void attachCamera(Camera& camera);
	void useUVMap(const std::string& filepath);
	void updateVAO();

	void useTextureMode();
	void useColorMode();

	~Mesh();

	static Mesh* makeBox(const std::string& name, float l, float w, float h, glm::vec3 pos = glm::vec3(0, 0, 0));
	static Mesh* makePlane(const std::string& name, float l, float w, glm::vec3 pos = glm::vec3(0, 0, 0));

private:
	unsigned int VAO, VBO, vertex_count;
	enum {
		TEXTURE,
		COLOR
	} texture_mode = TEXTURE;

	void createVAO();
};

#endif