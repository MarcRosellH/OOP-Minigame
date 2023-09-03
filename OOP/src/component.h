#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class Object;

enum class COMPONENT_TYPE
{
	COMPONENT_TRANSFORM,
	COMPONENT_MESH_RENDER,
	COMPONENT_CAMERA,
	COMPONENT_BEHAVIOUR,
	MAX
};

class Component
{
public:
	// Public methods
	Component(Object* _object) : object(_object) {}
	virtual ~Component() {}

	virtual void start() {}
	virtual void update() {}
	virtual void clean_up() {}

private:
	// Private methods

public:
	// Public data
	Object* object;

private:
	// Private data
};

#endif // !__COMPONENT_H__