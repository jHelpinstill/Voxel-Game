#include "ClickableRegion.h"

void ClickableRegion::performAction(Input* input)
{
	if (actionFunc)
		actionFunc(this, input);
}