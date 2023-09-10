#pragma once
#ifndef __APP_H__
#define __APP_H__

#include <chrono>
#include <vector>

#include "module.h"

#define MAX_TIME 10.0

enum Update_State;

class RenderModule;
class InputModule;
class ResourceManager;
class Scene;

typedef std::chrono::steady_clock Clock;

class App
{
public:
	// Modules declaration
	RenderModule* renderer = nullptr;
	InputModule* input = nullptr;

	float average_delta_time;
	float average_fps;

	float delta_time;

	std::vector<float> delta_time_list;

	Clock::time_point last_time; // High performance timer to overview application efficiency

	float total_time;
	float fps;
	unsigned int times_count;
private:
	// Private internal data
	bool quit;


	std::vector<Module*> modules;

	ResourceManager* resource;
	Scene* main_scene;

public:
	// Public methods
	App();
	~App();

	bool initialize();
	Update_State update();
	bool clean_up();

	void quit_app();

	Scene* get_scene();

private:
	// Private methods
	void module_add(Module* _module);
};

void on_glfw_error(int _error_code, const char* _error_message);

#endif // !__APP_H__