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
	shader->setMat4("projection", camera->getProjectionMat());
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
	vertex_count = 0;
	std::vector<float> data;
	for (Tri& tri : tris)
	{

		data.push_back(tri.verts[0].x); data.push_back(tri.verts[0].y); data.push_back(tri.verts[0].z);
		data.push_back(1.0f); data.push_back(0.0f); data.push_back(0.0f);

		data.push_back(tri.verts[1].x); data.push_back(tri.verts[1].y); data.push_back(tri.verts[1].z);
		data.push_back(0.0f); data.push_back(1.0f); data.push_back(0.0f);

		data.push_back(tri.verts[2].x); data.push_back(tri.verts[2].y); data.push_back(tri.verts[2].z);
		data.push_back(0.0f); data.push_back(0.0f); data.push_back(1.0f);
		
		vertex_count += 3;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	int stride = 6 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);

	//color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)stride);
	glEnableVertexAttribArray(1);
}

Mesh* Mesh::makeBox(float l, float w, float h, glm::vec3 pos)
{
	glm::vec3 v000(0.0f, 0.0f, 0.0f);
	glm::vec3 v00l(0.0f, 0.0f, l);
	glm::vec3 v0h0(0.0f, h, 0.0f);
	glm::vec3 v0hl(0.0f, h, l);
	glm::vec3 vw00(w, 0.0f, 0.0f);
	glm::vec3 vw0l(w, 0.0f, l);
	glm::vec3 vwh0(w, h, 0.0f);
	glm::vec3 vwhl(w, h, l);

	std::vector<Mesh::Tri> tris{
		//-x face
		Mesh::Tri(v000, v00l, v0h0),
		Mesh::Tri(v00l, v0hl, v0h0),
		//+z face
		Mesh::Tri(v00l, vw0l, v0hl),
		Mesh::Tri(vw0l, vwhl, v0hl),
		//+x face
		Mesh::Tri(vw0l, vw00, vwhl),
		Mesh::Tri(vw00, vwh0, vwhl),
		//-z face
		Mesh::Tri(vw00, v000, vwh0),
		Mesh::Tri(v000, v0h0, vwh0),
		//+y face
		Mesh::Tri(v0h0, v0hl, vwh0),
		Mesh::Tri(v0hl, vwhl, vwh0),
		//-y face
		Mesh::Tri(v000, vw0l, v00l),
		Mesh::Tri(v000, vw00, vw0l)
	};

	pos.x -= w / 2;
	pos.y -= h / 2;
	pos.z -= l / 2;

	Mesh* mesh = new Mesh(tris);
	mesh->transform = glm::translate(glm::mat4(1.0f), pos);
	return mesh;
}