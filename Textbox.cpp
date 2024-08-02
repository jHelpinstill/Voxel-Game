#include "Textbox.h"

void Textbox::drawTextbox(Decal* decal, GLFWwindow* window, void* obj)
{
	if(decal->texture)
	{
		decal->shader->use();

		decal->shader->setMat4("projection", decal->getMat());
		glBindTexture(GL_TEXTURE_2D, decal->texture);

		decal->vao->bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	Textbox* textbox = (Textbox*)obj;

	
	glm::vec2 cursor = decal->getScreenCoords();
	cursor.y += decal->size.y;
	long start = 0;
	while(start < textbox->text.size())
	{
		float current_width = 0;
		int i = 0;
		while (start + i < textbox->text.size())
		{
			current_width += textbox->font->getCharacter(textbox->text[start + i]).advance * textbox->scale / 64;
			i++;
			if (current_width >= decal->size.x)
			{
				i--;
				break;
			}
		}
		cursor.y -= textbox->font->height * textbox->scale;
		renderText(textbox->text.substr(start, i), textbox->font, textbox->shader, decal->window, cursor, textbox->scale, textbox->color);
		start += i;
	}
}