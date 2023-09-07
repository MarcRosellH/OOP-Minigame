#pragma once
#ifndef __BEHAVIOUR_H__
#define __BEHAVIOUR_H__
#include <glm/glm.hpp>

#include "utils.h"
#include "memleak.h"

#include "component.h"
#include "transform.h"
#include "object.h"

class ComponentBehaviour : public Component
{
public:
	// Public methods
	ComponentBehaviour(Object* _object);
	~ComponentBehaviour();

	void update(float dt);
private:
	// Private methods
	void set_to_delete();

public:
	// Public data
private:
	// Private data
	glm::vec3 vel;
};

#endif // !__BEHAVIOUR_H__