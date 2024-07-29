#include "HUDElement.h"

void HUDElement::draw()
{
	if (drawFunc)
		drawFunc(this, parent_obj);
}

