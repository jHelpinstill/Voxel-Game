#include "ObjectManager.h"

Mesh* createTexturedBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	const std::string& tex_name,
	const std::string& uv_filepath
) {
	Mesh* box = Mesh::makeBox(size, getTextureByName(tex_name), uv_filepath, pos);
	box->attachShader(getShaderByName("texture_shader"));

	meshes[name] = box;
	return box;
}

Mesh* createBox(
	const std::string& name,
	glm::vec3 size,
	glm::vec3 pos,
	glm::vec3 color
) {
	Mesh* box = Mesh::makeBox(size, 0, "", pos, color);
	box->attachShader(getShaderByName("color_shader"));

	meshes[name] = box;
	return box;
}

Mesh* createTexturedPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	const std::string& tex_name,
	const std::string& uv_filepath
) {
	Mesh* plane = Mesh::makePlane(size, getTextureByName(tex_name), uv_filepath, pos);
	plane->attachShader(getShaderByName("texture_shader"));

	meshes[name] = plane;
	return plane;
}

Mesh* createPlane(
	const std::string& name,
	glm::vec2 size,
	glm::vec3 pos,
	glm::vec3 color
) {
	Mesh* plane = Mesh::makePlane(size, 0, "", pos, color);
	plane->attachShader(getShaderByName("color_shader"));

	meshes[name] = plane;
	return plane;
}