#include "VAO.h"

void VAO::bind()
{
	glBindVertexArray(ID);
}

void VAO::makeTextured(const std::vector<glm::vec3>& verts, const std::vector<glm::vec2>& uv_coords)
{
	reset();
	if (verts.size() != uv_coords.size())
	{
		std::cout << "Vertex and UV_coords size mismatch (";
		std::cout << verts.size() << ", " << uv_coords.size() << "), aborting VAO creation" << std::endl;
		style = Style::NONE;
		return;
	}
	style = Style::TEXTURED;

	std::vector<float> data;
	for (int vert = 0; vert < verts.size(); vert++)
	{
		for (int i = 0; i < 3; i++)
			data.push_back(verts[vert][i]);

		data.push_back(uv_coords[vert].x);
		data.push_back(uv_coords[vert].y);
	}

	glGenVertexArrays(1, &ID);
	bind();

	glGenBuffers(1, &verts_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, verts_VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	int stride = 5 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void VAO::makeSolidColored(const std::vector<glm::vec3>& verts, const glm::vec3& color)
{
	reset();
	style = Style::SOLID_COLORED;

	std::vector<float> data;
	for (const glm::vec3& vert : verts)
	{
		for (int i = 0; i < 3; i++)
			data.push_back(vert[i]);
	}

	glGenVertexArrays(1, &ID);
	bind();

	glGenBuffers(1, &verts_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, verts_VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	int stride = 3 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
}

void VAO::makeInstanced(const std::vector<glm::vec3>& verts, const std::vector<int>& instance_data)
{
	reset();
	style = Style::INSTANCED;

	std::vector<float> vert_data;
	for (int vert = 0; vert < verts.size(); vert++)
	{
		for (int i = 0; i < 3; i++)
			vert_data.push_back(verts[vert][i]);
	}

	glGenVertexArrays(1, &ID);
	bind();

	glGenBuffers(1, &verts_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, verts_VBO);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size() * sizeof(float), vert_data.data(), GL_STATIC_DRAW);

	int stride = 3 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	// instance information
	glGenBuffers(1, &data_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, data_VBO);
	glBufferData(GL_ARRAY_BUFFER, instance_data.size() * sizeof(int), instance_data.data(), GL_STATIC_DRAW);

	glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), (void*)0);
	glVertexAttribDivisor(1, 1);
	glEnableVertexAttribArray(1);
}

void VAO::makeDecal()
{
	reset();

	style = Style::DECAL;

	std::vector<float> data;
	const glm::vec2 verts[4] =
	{
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(0, 1),
		glm::vec2(1, 1)
	};

	for (int i = 0; i < 4; i++)
	{
		data.push_back(verts[i].x);
		data.push_back(verts[i].y);
		data.push_back(verts[i].x);
		data.push_back(verts[i].y);
	}

	glGenVertexArrays(1, &ID);
	bind();

	glGenBuffers(1, &verts_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, verts_VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

	int stride = 4 * sizeof(float);

	// position/texture vec4 (posx, posy, uvx, uvy)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
}

void VAO::reset()
{
	switch(style)
	{
	case Style::INSTANCED:
	case Style::DECAL:
		glDeleteBuffers(1, &data_VBO);
	default:
		glDeleteVertexArrays(1, &ID);
		glDeleteBuffers(1, &verts_VBO);
	case Style::NONE:
		break;
	}
}

VAO::~VAO()
{
	glDeleteBuffers(1, &data_VBO);
	glDeleteVertexArrays(1, &ID);
	glDeleteBuffers(1, &verts_VBO);
}