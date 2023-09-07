#pragma once

#include "glfw3.h"

#include <iostream>

#include "app.h"
#include "utils.h"
#include "memleak.h"
#include "module.h"

#include "input.h"
#include "render.h"

#include "resource_manager.h"
#include "scene.h"

App::App() : quit(false), average_delta_time(0.F), average_fps(0.F), delta_time(0.F), total_time(0.0), times_count(1), fps(0.F), total_created(0), total_destroyed(0)
{
	//input = DBG_NEW InputModule(this);
	resource = DBG_NEW ResourceManager(this);

	//module_add(input);

	main_scene = DBG_NEW Scene(this,resource);
	//renderer = DBG_NEW RenderModule(this, resource, main_scene);
	//module_add(renderer);

}

App::~App()
{
	for (int i = modules.size() - 1; i >= 0; --i)
	{
		delete modules[i];
		modules[i] = nullptr;
	}
	modules.clear();

	if (resource != nullptr) RELEASE(resource);
	if (main_scene != nullptr) RELEASE(main_scene);
}

bool App::initialize()
{
	bool ret = true;

	LOG("Initializing application modules...");
	for (unsigned int i = 0; i < modules.size() && ret; ++i)
	{
		ret = modules[i]->initialize();
	}

	resource->initial_load();

	LOG("Starting application modules...");
	for (unsigned int i = 0; i < modules.size() && ret; ++i)
	{
		ret = modules[i]->start();
	}

	last_time = Clock::now();

	return ret;
}

Update_State App::update()
{
	auto now = Clock::now();
	Update_State ret = UPDATE_CONTINUE;

	for (unsigned int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modules[i]->pre_update();
	}
	main_scene->update();
	for (unsigned int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modules[i]->update();
	}
	for (unsigned int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modules[i]->post_update();
	}

	delta_time = std::chrono::duration<float,std::chrono::seconds::period>(now - last_time).count();
	total_time += delta_time;
#ifdef RELEASE
	std::cout << delta_time << std::endl;
#endif // RELEASE


	average_delta_time = average_delta_time + ((delta_time - average_delta_time) / times_count);
	fps = 1 / delta_time;
	average_fps = average_fps + ((fps - average_fps) / times_count);
	times_count++;

	delta_time_list.push_back(delta_time);
	last_time = now;
	quit = (total_time > MAX_TIME) ? true : false;
	return (quit) ? UPDATE_STOP : ret;
}

bool App::clean_up()
{
	bool ret = true;
	total_created = main_scene->total_created;
	total_destroyed = main_scene->total_destroyed;

	for (int i = modules.size() - 1; i >= 0 && ret; --i)
	{
		ret = modules[i]->clean_up();
		delete modules[i];
	}
	modules.clear();

	RELEASE(resource);
	resource = nullptr;


	main_scene->clean_up();

	RELEASE(main_scene);
	main_scene = nullptr;

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