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
	unsigned int VAO, VBO, VBO_instanced, SSBO;
	void getUVMap(const std::string& filepath);

	Transform transform;
	Shader* shader;

	Shader::VAOStyle style;
	unsigned int texture;
	std::string uv_filepath;
	glm::vec3 color;
	void (*drawFunction)(Mesh* mesh, Camera* camera);

	std::vector<glm::vec3> verts;
	std::vector<int> instance_data;
	std::vector<glm::vec2> uv_coords;

	Mesh(unsigned int texture, void (*drawFunction)(Mesh*, Camera*) = drawTriangles);
	Mesh(
		const std::vector<glm::vec3>& verts,
		unsigned int texture,
		const std::string& uv_filepath,
		void (*drawFunction)(Mesh*, Camera*) = drawTriangles
	);
	Mesh(
		const std::vector<glm::vec3>& verts,
		unsigned int texture,
		const std::vector<glm::vec2>& uv_coords,
		void (*drawFunction)(Mesh*, Camera*) = drawTriangles
	);
	Mesh(
		const std::vector<glm::vec3>& verts,
		glm::vec3 color = glm::vec3(0.5, 0.5, 0.5),
		void (*drawFunction)(Mesh*, Camera*) = drawTriangles
	);
	
	~Mesh();

	void draw(Camera* camera);
	void attachShader(Shader* shader);
	void deleteVAO();
	void remakeVAO();
	void generateInstancedVAO();

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
	
	static void drawTriangles(Mesh* mesh, Camera* camera);
	static void drawInstancedStrip(Mesh* mesh, Camera* camera);
};

#endif