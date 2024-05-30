#pragma once
#ifndef MESH_
#define MESH_

#include "config.h"
#include "Camera.h"
#include "Shader.h"
#include "Transform.h"

class Mesh
{
private:
	unsigned int VAO, VBO;

public:
	std::string name;
	Transform transform;
	Shader* shader;

	Shader::VAOStyle style;
	unsigned int texture;
	std::string uv_filepath;
	glm::vec3 color;

	std::vector<glm::vec3> verts;

	Mesh(
		const std::string& name,
		const std::vector<glm::vec3>& verts,
		unsigned int texture,
		const std::string& uv_filepath
	);
	Mesh(
		const std::string& name,
		const std::vector<glm::vec3>& verts,
		glm::vec3 color = glm::vec3(0.5, 0.5, 0.5)
	);

	void draw(Camera* camera);
	void attachShader(Shader* shader);

	~Mesh();

	static Mesh* makeBox(
		const std::string& name,
		const glm::vec3& size,
		unsigned int texture,
		const std::string& uv_filepath,
		const glm::vec3& pos = glm::vec3(0, 0, 0),
		const glm::vec3& color = glm::vec3(0.5, 0.5, 0.5)
	);
	static Mesh* makePlane(
		const std::string& name,
		const glm::vec2& size,
		unsigned int texture,
		const std::string& uv_filepath,
		const glm::vec3& pos = glm::vec3(0, 0, 0),
		const glm::vec3& color = glm::vec3(0.5, 0.5, 0.5)
	);
};

#endif