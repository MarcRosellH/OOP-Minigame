#pragma once

#include "behaviour.h"
#include "scene.h"

ComponentBehaviour::ComponentBehaviour(Object* _object) : Component(_object)
{
	vel = glm::vec3(
		((rand() % 10) + 1)*(rand()%3-1),
		((rand() % 10) + 1) * (rand() % 3 - 1),
		((rand() % 10) + 1) * (rand() % 3 - 1));
}

ComponentBehaviour::~ComponentBehaviour()
{
}

void ComponentBehaviour::update()
{
	glm::vec3 aux = object->transform->get_position();
	object->transform->set_position(glm::vec3(aux.x + vel.x, aux.y + vel.y, aux.z + vel.z));
	if (abs(aux.x) > 70 || abs(aux.y) > 70 || abs(aux.z) > 70)
	{
		set_to_delete();
	}
}

void ComponentBehaviour::set_to_delete()
{
	object->get_scene()->to_delete_objects.push_back(object);
}
