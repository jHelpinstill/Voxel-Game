#pragma once
#ifndef VAO_
#define VAO_

#include "Config.h"

class VAO
{
public:
	enum Style
	{
		TEXTURED,
		SOLID_COLORED,
		INSTANCED,
		NONE
	} style;
	unsigned int ID, verts_VBO, data_VBO;

	VAO() : style(Style::NONE), ID(0), verts_VBO(0), data_VBO(0) {}
	~VAO();

	void bind();

	void makeTextured(const std::vector<glm::vec3>& verts, const std::vector<glm::vec2>& uv_coords);
	void makeSolidColored(const std::vector<glm::vec3>& verts, const glm::vec3& color);
	void makeInstanced(const std::vector<glm::vec3>& verts, const std::vector<int>& instance_data);

	void reset();
};

#endif