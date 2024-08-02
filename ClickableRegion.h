#pragma once
#ifndef CLICKABLE_REGION
#define CLICKABLE_REGION

#include "Config.h"
#include "Decal.h"
#include "TextBox.h"
#include "Input.h"

class ClickableRegion
{
	Decal* decal;
	void* attached_obj;

	void (*actionFunc)(ClickableRegion* cr, Input* input);

	ClickableRegion() {}

	void performAction(Input* input);

};

#endif