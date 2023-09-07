#pragma once

#include "behaviour.h"
#include "scene.h"

ComponentBehaviour::ComponentBehaviour(Object* _object) : Component(_object)
{
	vel = glm::vec3(
		(((rand() - (RAND_MAX / 2)) % 10)),
		(((rand() - (RAND_MAX / 2)) % 10)),
		(((rand() - (RAND_MAX / 2)) % 10)));
	if (vel.x == 0.F && vel.y == 0.F && vel.z == 0.F)
	{
		set_to_delete();
	}
}

ComponentBehaviour::~ComponentBehaviour()
{
}

void ComponentBehaviour::update(float dt)
{
	glm::vec3 aux = object->transform->get_position();
	object->transform->set_position(glm::vec3(aux.x + vel.x * dt, aux.y + vel.y * dt, aux.z + vel.z * dt));
	if (abs(aux.x) > 10 || abs(aux.y) > 10 || abs(aux.z) > 10)
	{
		set_to_delete();
	}
}

void ComponentBehaviour::set_to_delete()
{
	object->get_scene()->to_delete_objects.push_back(object);
}
