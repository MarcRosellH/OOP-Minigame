#pragma once
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

class Object;

class Component
{
public:
	// Public methods
	Component(Object* _object);
	virtual ~Component();


private:
	// Private methods

public:
	// Public data

private:
	// Private data
	Object* object;
};

#endif // !__COMPONENT_H__