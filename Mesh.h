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
	void getUVMap(const std::string& filepath);

	static void meshDrawTriangles(Mesh* mesh, Camera* camera);

public:
	Transform transform;
	Shader* shader;

	Shader::VAOStyle style;
	unsigned int texture;
	std::string uv_filepath;
	glm::vec3 color;
	void (*drawFunction)(Mesh* mesh, Camera* camera);

	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uv_coords;

	Mesh(unsigned int texture, void (*drawFunction)(Mesh*, Camera*) = meshDrawTriangles);
	Mesh(
		const std::vector<glm::vec3>& verts,
		unsigned int texture,
		const std::string& uv_filepath,
		void (*drawFunction)(Mesh*, Camera*) = meshDrawTriangles
	);
	Mesh(
		const std::vector<glm::vec3>& verts,
		unsigned int texture,
		const std::vector<glm::vec2>& uv_coords,
		void (*drawFunction)(Mesh*, Camera*) = meshDrawTriangles
	);
	Mesh(
		const std::vector<glm::vec3>& verts,
		glm::vec3 color = glm::vec3(0.5, 0.5, 0.5),
		void (*drawFunction)(Mesh*, Camera*) = meshDrawTriangles
	);
	
	~Mesh();

	void draw(Camera* camera);
	void attachShader(Shader* shader);
	void deleteVAO();
	void remakeVAO();

	static Mesh* makeBox(
		const glm::vec3& size,
		unsigned int texture,
		const std::string& uv_filepath,
		const glm::vec3& pos = glm::vec3(0, 0, 0),
		const glm::vec3& color = glm::vec3(0.5, 0.5, 0.5)
	);
	static Mesh* makePlane(
		const glm::vec2& size,
		unsigned int texture,
		const std::string& uv_filepath,
		const glm::vec3& pos = glm::vec3(0, 0, 0),
		const glm::vec3& color = glm::vec3(0.5, 0.5, 0.5)
	);
};

#endif