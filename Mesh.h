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

	Shader* shader;
	Transform transform;

	unsigned int texture;
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

	void draw(Camera* camera);

	void attachShader(Shader* shader);
	void useUVMap(const std::string& filepath);
	void updateVAO();

	void useTextureMode();
	void useTextureMode(unsigned int texture);
	void useColorMode();
	void useColorMode(const glm::vec3& color);

	~Mesh();

	static Mesh* makeBox(const std::string& name, float l, float w, float h, const glm::vec3& pos = glm::vec3(0, 0, 0));
	static Mesh* makePlane(const std::string& name, float l, float w, const glm::vec3& pos = glm::vec3(0, 0, 0));

private:
	unsigned int VAO, VBO, vertex_count;
	enum {
		TEXTURE,
		COLOR
	} shader_mode = TEXTURE;

	void createTextureVAO();
	void createSolidColorVAO();
};

#endif