#pragma once

#include "module.h"
#include "utils.h"
#include "memleak.h"

#include "input.h"
#include <glfw3.h>

#include "app.h"

InputModule::InputModule(App* _app) : Module(_app)
{
}

InputModule::~InputModule()
{
}

Update_State InputModule::pre_update()
{

	glfwPollEvents();

	return UPDATE_CONTINUE;
}

Update_State InputModule::post_update()
{
	if (mouse_buttons[LEFT] != BUTTON_IDLE)
	{
		LOG("Letter A state is now: %d", (int)mouse_buttons[LEFT]);
	}

	for (size_t i = 0; i < MOUSE_BUTTON_COUNT; ++i)
	{
		if (mouse_buttons[i] == BUTTON_PRESS)
		{
			mouse_buttons[i] = BUTTON_PRESSED;
		}
		else if (mouse_buttons[i] == BUTTON_RELEASE)
		{
			mouse_buttons[i] = BUTTON_IDLE;
		}
	}

	for (size_t i = 0; i < KEY_COUNT; ++i)
	{
		if (keys[i] == BUTTON_PRESS)
		{
			keys[i] = BUTTON_PRESSED;
		}
		else if (keys[i] == BUTTON_RELEASE)
		{
			keys[i] = BUTTON_IDLE;
		}
	}

	return UPDATE_CONTINUE;
}
