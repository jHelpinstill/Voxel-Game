#include "Textbox.h"

void Textbox::drawTextbox(Decal* decal, GLFWwindow* window)
{
	if(decal->texture)
		Decal::drawDefault(decal, window);

	Textbox* textbox = (Textbox*)decal->attached_obj;

	glm::vec2 cursor = decal->getScreenCoords();
	cursor.y += decal->size.y;
	long start = 0;
	while(start < textbox->text.size())
	{
		float current_width = 0;
		int i = 0;
		while (start + i < textbox->text.size())
		{
			if (textbox->text[start + i] == '\n')
			{
				i++;
				break;
			}
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