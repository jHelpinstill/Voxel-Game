#include "Game.h"

void Game::createTexturedBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	const std::string& tex_name,
	const std::string& uv_filepath
) {
	Mesh* box = Mesh::makeBox(name, size, getTextureByName(tex_name), uv_filepath, pos);
	box->attachShader(getShaderByName("texture_shader"));

	meshes[name] = box;
}

void Game::createBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	glm::vec3 color
) {
	Mesh* box = Mesh::makeBox(name, size, 0, "", pos, color);
	box->attachShader(getShaderByName("color_shader"));

	meshes[name] = box;
}

void Game::createTexturedPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	const std::string& tex_name,
	const std::string& uv_filepath
) {
	Mesh* plane = Mesh::makePlane(name, size, getTextureByName(tex_name), uv_filepath, pos);
	plane->attachShader(getShaderByName("texture_shader"));

	meshes[name] = plane;
}

void Game::createPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	glm::vec3 color
) {
	Mesh* plane = Mesh::makePlane(name, size, 0, "", pos, color);
	plane->attachShader(getShaderByName("color_shader"));

	meshes[name] = plane;
}