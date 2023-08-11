#pragma once
#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>

class Component;
enum class COMPONENT_TYPE;

class Object
{
public:
	// Public methods
	Object(bool _active);
	~Object();

	Component* add_component(COMPONENT_TYPE);

	void start();
	void update();
	void clean_up();

	void set_active(bool);

private:
	// Private methods

public:
	// Public data

private:
	// Private data
	std::vector<Component*> components;
	bool active;
};

#endif // !__OBJECT_H__