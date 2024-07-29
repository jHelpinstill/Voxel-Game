#pragma once
#ifndef MESH_
#define MESH_

#include "config.h"
#include "Camera.h"
#include "Shader.h"
#include "Transform.h"
#include "VAO.h"

class Mesh
{
public:
	VAO* vao = nullptr;
	void getUVMap(const std::string& filepath);

	Transform transform;
	Shader* shader;
	void* parent_obj = nullptr;

	Shader::VAOStyle style;
	unsigned int texture;
	std::string uv_filepath;
	glm::vec3 color;
	void (*drawFunc)(Mesh* mesh, Camera* camera, void* obj);

	std::vector<glm::vec3> verts;
	std::vector<int> instance_data;
	std::vector<glm::vec2> uv_coords;

	Mesh(unsigned int texture, void (*drawFunction)(Mesh*, Camera*, void*) = drawTriangles);
	Mesh(
		const std::vector<glm::vec3>& verts,
		unsigned int texture,
		const std::string& uv_filepath,
		void (*drawFunction)(Mesh*, Camera*, void*) = drawTriangles
	);
	Mesh(
		const std::vector<glm::vec3>& verts,
		unsigned int texture,
		const std::vector<glm::vec2>& uv_coords,
		void (*drawFunc)(Mesh*, Camera*, void*) = drawTriangles
	);
	Mesh(
		const std::vector<glm::vec3>& verts,
		glm::vec3 color = glm::vec3(0.5, 0.5, 0.5),
		void (*drawFunc)(Mesh*, Camera*, void*) = drawTriangles
	);
	
	~Mesh();

	void draw(Camera* camera);
	void attachShader(Shader* shader);
	//void deleteVAO();
	//void remakeVAO();
	//void generateInstancedVAO();

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
	
	static void drawTriangles(Mesh* mesh, Camera* camera, void* obj);
	static void drawInstancedStrip(Mesh* mesh, Camera* camera, void* obj);
};

#endif