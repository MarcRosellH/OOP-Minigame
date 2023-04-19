#pragma once
#ifndef __MODULE_H__
#define __MODULE_H__

class App;

enum Update_State
{
	UPDATE_CONTINUE = 0,
	UPDATE_ERROR,
	UPDATE_STOP,
};

class Module
{
public:
	Module(App* _app_root) : app(_app_root) {}

	virtual ~Module() {}

	virtual bool initialize() { return true; }

	virtual bool start() { return true; }

	virtual Update_State pre_update() { return UPDATE_CONTINUE; }

	virtual Update_State update() { return UPDATE_CONTINUE; }

	virtual Update_State post_update() { return UPDATE_CONTINUE; }

	virtual bool clean_up() { return true; }

public:
	App* app;
};

#endif // !__MODULE_H__