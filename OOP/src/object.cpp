#include "memleak.h"
#include "utils.h"

#include "object.h"
#include "component.h"
#include "transform.h"
#include "mesh_render.h"

Object::Object(bool _active) : active(_active)
{
	Component* transform_component_aux = DBG_NEW ComponentTransform(this);
	components.push_back(transform_component_aux);
}

Object::~Object()
{
}

Component* Object::add_component(COMPONENT_TYPE _type)
{
	Component* new_component = nullptr;

	switch (_type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		LOG("Transform component already exists by default");
		break;
	case COMPONENT_TYPE::COMPONENT_MESH_RENDER:
		new_component = DBG_NEW ComponentMeshRender(this);
		break;
	default:
		LOG("Component with type %d, could not be created correctly", _type);
		break;
	}

	return new_component;
}

void Object::start()
{
	for (int i = 0; active && i < components.size(); ++i)
	{
		components[i]->start();
	}
}

void Object::update()
{
	for (int i = 0; active && i < components.size(); ++i)
	{
		components[i]->update();
	}
}

void Object::clean_up()
{
	active = false;
	for (int i = components.size() - 1; i >= 0; --i)
	{
		components[i]->clean_up();
		RELEASE(components[i]);
	}
	components.clear();
}

void Object::set_active(bool _active)
{
	active = _active;
}
