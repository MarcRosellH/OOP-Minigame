#pragma once

#include "glfw3.h"

#include "app.h"
#include "utils.h"
#include "memleak.h"
#include "module.h"

#include "input.h"
#include "render.h"

App::App() : quit(false)
{
	input = DBG_NEW InputModule(this);
	renderer = DBG_NEW RenderModule(this);

	module_add(input);

	module_add(renderer);
}

App::~App()
{
	for (int i = modules.size() - 1; i >= 0; --i)
	{
		delete modules[i];
		modules[i] = nullptr;
	}
	modules.clear();
}

bool App::initialize()
{
	bool ret = true;

	LOG("Initializing application modules...");
	for (int i = 0; i < modules.size() && ret; ++i)
	{
		ret = modules[i]->initialize();
	}
	LOG("Starting application modules...");
	for (unsigned int i = 0; i < modules.size() && ret; ++i)
	{
		ret = modules[i]->start();
	}

	return ret;
}

Update_State App::update()
{
	Update_State ret = UPDATE_CONTINUE;

	for (unsigned int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modules[i]->pre_update();
	}
	for (unsigned int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modules[i]->update();
	}
	for (unsigned int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modules[i]->post_update();
	}

	return (quit) ? UPDATE_STOP : ret;
}

bool App::clean_up()
{
	bool ret = true;

	for (int i = modules.size() - 1; i >= 0 && ret; --i)
	{
		ret = modules[i]->clean_up();
	}

	ASSERT(ret, "App clean up failed!");

	return ret;
}

void App::module_add(Module* _module)
{
	modules.push_back(_module);
}

void App::quit_app()
{
	this->quit = true;
}

void on_glfw_error(int _error_code, const char* _error_message)
{
	fprintf(stderr, "Glfw failed with error %d: %s\n", _error_code, _error_message);
}