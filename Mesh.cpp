#include "Mesh.h"

Mesh::Mesh(std::vector<Tri>& tris)
{
	for (Tri tri : tris)
	{
		setNorm(tri);
		this->tris.push_back(tri);
	}
	transform = glm::mat4(1.0f);
	createVAO();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Mesh::draw()
{
	shader->use();
	shader->setMat4("projection", camera->getMat());
	shader->setMat4("transform", transform);
	shader->setVec4("color", color);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

void Mesh::attachShader(Shader& shader)
{
	this->shader = &shader;
}

void Mesh::attachCamera(Camera& camera)
{
	this->camera = &camera;
}

void Mesh::setNorm(Tri& tri)
{
	tri.norm = glm::cross(tri.verts[0], tri.verts[1]);
}

void Mesh::createVAO()
{
	std::vector<float> data;
	for (Tri& tri : tris)
	{
		for (int i = 0; i < 3; i++)
		{
			data.push_back(tri.verts[i].x);
			data.push_back(tri.verts[i].y);
			data.push_back(tri.verts[i].z);
			vertex_count += 3;
		}
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	int stride = 3 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
}