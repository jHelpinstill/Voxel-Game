#include "Decal.h"

Decal::Decal(unsigned int texture, const glm::vec2 &size, const glm::vec2 &pos)
	: texture(texture), drawFunc(drawDefault), size(size), pos(pos) {
	vao = new VAO;
	vao->makeDecal();
	origin = glm::vec2(0.0f);
	adjustment = glm::vec2(0.0f);
}
#include "util.h"

void Decal::draw() {
	//std::cout << "decal draw called" << std::endl;
	if (drawFunc && window && awake)
		drawFunc(this, window);
}

glm::vec2 Decal::getScreenCoords() {
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	return adjustment * glm::vec2(width, height) + pos - origin * size;
}

glm::mat4 Decal::getMat(bool with_proj) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glm::vec2 translation = adjustment * glm::vec2(width, height) + pos - origin * size;
	glm::mat4 mat = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(translation, 0.0f)), glm::vec3(size, 1.0f));
	if (with_proj)
		mat = glm::ortho(0.0f, (float)width, 0.0f, (float)height) * mat;
	return mat;
}

void Decal::drawDefault(Decal *decal, GLFWwindow *window) {
	decal->shader->use();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);

	decal->shader->setMat4("projection", decal->getMat());
	decal->vao->bind();

	glBindTexture(GL_TEXTURE_2D, decal->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

Decal::~Decal() {
	delete vao;
}