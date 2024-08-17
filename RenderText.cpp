#include "RenderText.h"

void renderText(
	const std::string& text,
	Font* font,
	Shader* shader,
	GLFWwindow* window,
	const glm::vec2& pos,
	float scale,
	const glm::vec3& color
) {
	VAO vao;
	vao.makeFont();

	shader->use();
	shader->setVec3("text_color", color);

	glm::vec2 cursor = pos;
	int window_w, window_h;
	glfwGetWindowSize(window, &window_w, &window_h);
	glm::mat4 projection = glm::ortho(0.0f, (float)window_w, 0.0f, (float)window_h);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vao.bind();
	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < text.size(); i++)
	{
		switch (text[i])
		{
		case '\n':
			continue;
		}
		Font::Character character = font->getCharacter(text[i]);

		glm::vec2 c_pos = cursor;
		c_pos.x += character.bearing.x * scale;
		c_pos.y -= (character.size.y - character.bearing.y) * scale;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(c_pos, 0.0f));
		transform = glm::scale(transform, glm::vec3(character.size.x * scale, character.size.y * scale, 1.0f));

		shader->setMat4("projection", projection * transform);
		glBindTexture(GL_TEXTURE_2D, character.texture);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		cursor.x += character.advance * scale / 64;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}