#include "HUDElement.h"

void HUDElement::draw()
{
	shader->use();
	shader->setVec4("color", color);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

void HUDElement::createVAO()
{
	std::vector<float> vert_data;

	for (int vert = 0; vert < verts.size(); vert++)
		for (int i = 0; i < 3; i++)
			vert_data.push_back(verts[vert][i]);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vert_data.size() * sizeof(float), vert_data.data(), GL_STATIC_DRAW);

	int stride = 3 * sizeof(float);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
}

void HUDElement::addRect(float x, float y, float w, float h)
{
	verts.push_back(glm::vec3(x, y, 0));
	verts.push_back(glm::vec3(x + w, y, 0));
	verts.push_back(glm::vec3(x, y + h, 0));
	verts.push_back(glm::vec3(x + w, y, 0));
	verts.push_back(glm::vec3(x + w, y + h, 0));
	verts.push_back(glm::vec3(x, y + h, 0));
}