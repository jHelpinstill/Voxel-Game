#include "Decal.h"

Decal::Decal(unsigned int texture, const glm::vec2& size, const glm::vec2& pos)
	: texture(texture), drawFunc(drawAbsolute), size(size), pos(pos)
{
	vao = new VAO;
	vao->makeDecal();
}
#include "util.h"

void Decal::draw()
{
	//std::cout << "decal draw called" << std::endl;
	if (drawFunc && window)
		drawFunc(this, window, parent_obj);
}



void Decal::drawAbsolute(Decal* decal, GLFWwindow* window, void* obj)
{
	decal->shader->use();

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(decal->pos, 0.0f));
	mat = glm::scale(mat, glm::vec3(decal->size, 1.0f));

	decal->shader->setMat4("projection", glm::ortho(0.0f, (float)width, 0.0f, (float)height) * mat);
	glBindTexture(GL_TEXTURE_2D, decal->texture);

	decal->vao->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Decal::drawRelative(Decal* decal, GLFWwindow* window, void* obj)
{
	decal->shader->use();

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float half_width = (float)width / 2;
	float half_height = (float)height / 2;

	glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(decal->pos, 0.0f));
	mat = glm::scale(mat, glm::vec3(decal->size, 1.0f));

	decal->shader->setMat4("projection", glm::ortho(-half_width, half_width, -half_height, half_height) * mat);
	glBindTexture(GL_TEXTURE_2D, decal->texture);

	decal->vao->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Decal::drawCentered(Decal* decal, GLFWwindow* window, void* obj)
{
	decal->shader->use();

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float half_width = (float)width / 2;
	float half_height = (float)height / 2;

	glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(decal->pos - (decal->size * 0.5f), 0.0f));
	mat = glm::scale(mat, glm::vec3(decal->size, 1.0f));

	decal->shader->setMat4("projection", glm::ortho(-half_width, half_width, -half_height, half_height) * mat);
	glBindTexture(GL_TEXTURE_2D, decal->texture);

	decal->vao->bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

Decal::~Decal()
{
	delete vao;
}