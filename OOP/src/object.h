#pragma once
#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <vector>

class Component;

class Object
{
public:
	// Public methods
	Object();
	~Object();

	Component* add_component();

	void update();

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