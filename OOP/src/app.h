#pragma once
#ifndef __APP_H__
#define __APP_H__

#include <chrono>
#include <vector>

#include "module.h"

enum Update_State;

class RenderModule;
class InputModule;

class App
{
public:
	// Modules declaration
	RenderModule* renderer = nullptr;
	InputModule* input = nullptr;

private:
	// Private internal data
	bool quit;

	std::chrono::steady_clock::time_point frame_begin;	// High performance timer to overview application efficiency
	std::vector<Module*> modules;


public:
	// Public methods
	App();
	~App();

	bool initialize();
	Update_State update();
	bool clean_up();

	void quit_app();

private:
	// Private methods
	void module_add(Module* _module);
};

void on_glfw_error(int _error_code, const char* _error_message);

#endif // !__APP_H__