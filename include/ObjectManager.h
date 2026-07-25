#pragma once
#ifndef OBJECT_MANAGER
#define OBJECT_MANAGER

#include "config.h"
#include "Mesh.h"
#include "Decal.h"
#include "Font.h"

extern std::unordered_map<std::string, Mesh*> meshes;
extern std::unordered_map<std::string, Shader*> shaders;
extern std::unordered_map<std::string, unsigned int> textures;
extern std::unordered_map<std::string, Decal*> decals;
extern std::unordered_map<std::string, Font*> fonts;

Shader* getShaderByName(const std::string& name);
Mesh* getMeshByName(const std::string& name);
unsigned int getTextureByName(const std::string& name);
Decal* getDecalByName(const std::string& name);
Font* getFontByName(const std::string& name);

void removeShader(const std::string& name);
void removeMesh(const std::string& name);
void removeTexture(const std::string& name);
void removeDecal(const std::string& name);
void removeFont(const std::string& name);

Shader* createShader(
	const std::string& name,
	const std::string& vertex_filepath,
	const std::string& fragment_filepath
);

unsigned int createTexture(
	const std::string& name,
	const std::string& filepath,
	bool alpha_channel = false
);

Decal* createDecal(
	const std::string& name,
	const std::string& tex_name,
	const std::string& shader_name,
	const glm::vec2& size,
	const glm::vec2& pos,
	GLFWwindow* window
);

// Tools for creating mesh primatives
Mesh* createTexturedBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	const std::string& tex_filepath,
	const std::string& uv_filepath = "meshes/box_one_face_UV.txt"
);
Mesh* createBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	glm::vec3 color = glm::vec3(0.5, 0.5, 0.5)
);

Mesh* createTexturedPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	const std::string& tex_filepath,
	const std::string& uv_filepath = "meshes/box_one_face_UV.txt"
);

Mesh* createPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	glm::vec3 color = glm::vec3(0.5, 0.5, 0.5)
);

#endif