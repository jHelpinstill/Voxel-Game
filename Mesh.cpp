#include "Mesh.h"

Mesh::Mesh(std::vector<Tri>& tris)
{
	for (Tri tri : tris)
	{
		setNorm(tri);
		this->tris.push_back(tri);
	}
}

void Mesh::draw()
{


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
		}
	}
}