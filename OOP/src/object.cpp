#pragma once
#include "memleak.h"
#include "utils.h"

#include "scene.h"

#include "object.h"
#include "component.h"
#include "transform.h"
#include "mesh_render.h"
#include "camera.h"
#include "behaviour.h"

Object::Object(Scene* _scene, bool _active) : scene(_scene), active(_active), parent(nullptr)
{
	/*Component* transform_component_aux = DBG_NEW ComponentTransform(this);
	components.push_back(transform_component_aux);*/
	scene->total_count++;
}

Object::~Object()
{
	scene->total_count--;
	clean_up();
}

Component* Object::add_component(COMPONENT_TYPE _type)
{
	Component* new_component = nullptr;
	switch (_type)
	{
	case COMPONENT_TYPE::COMPONENT_TRANSFORM:
		transform = DBG_NEW	ComponentTransform(this);
		components.push_back(transform);
		new_component = (Component*) transform;
		break;
	case COMPONENT_TYPE::COMPONENT_MESH_RENDER:
		new_component = DBG_NEW ComponentMeshRender(this);
		components.push_back(new_component);
		break;
	case COMPONENT_TYPE::COMPONENT_CAMERA:
		scene->camera_ref = DBG_NEW ComponentCamera(this);
		new_component = scene->camera_ref;
		components.push_back(new_component);
		break;
	case COMPONENT_TYPE::COMPONENT_BEHAVIOUR:
		new_component = DBG_NEW ComponentBehaviour(this);
		components.push_back(new_component);
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

void Object::update(float dt)
{
	for (int i = 0; active && i < components.size(); ++i)
	{
		components[i]->update(dt);
	}

	for (int i = 0; i < child.size(); ++i)
	{
		child[i]->update(dt);
	}
	scene->object_count++;
	if (scene->total_count < MAX_OBJECTS)
	{
		Object* ref = add_child();
		ref->set_type(OBJECT_TYPE::ENEMY);
		ref->add_component(COMPONENT_TYPE::COMPONENT_TRANSFORM);
		ref->add_component(COMPONENT_TYPE::COMPONENT_MESH_RENDER)->start();
		ref->add_component(COMPONENT_TYPE::COMPONENT_BEHAVIOUR);
		ref->transform->set_position(glm::vec3(1.F, 1.F, 1.F));
	}
}

void Object::clean_up()
{
	for (int i = components.size() - 1; i >= 0; --i)
	{
		components[i]->clean_up();
		RELEASE(components[i]);
	}
	components.clear();

	for (int i = child.size() - 1; i >= 0; --i)
	{
		child[i]->clean_up();
		RELEASE(child[i]);
		child.pop_back();
	}
	child.clear();
}

void Object::set_active(bool _active)
{
	active = _active;
}

Object* Object::add_child()
{
	Object* new_child = DBG_NEW Object(scene, true);
	new_child->set_parent(this);
	child.push_back(new_child);
	return new_child;
}

void Object::delete_child(Object* _ref)
{
	for (std::vector<Object*>::const_iterator it = child.begin(); it != child.end(); it++)
	{
		if ((*it) == _ref)
		{
			child.erase(it);
			RELEASE(_ref);
			return;
		}
	}
}

void Object::set_parent(Object* _ref)
{
	parent = _ref;
}

Object* Object::get_parent() const
{
	return parent;
}

OBJECT_TYPE Object::get_type() const
{
	return type;
}

void Object::set_type(OBJECT_TYPE _type)
{
	type = _type;
}

Scene* Object::get_scene() 
{
	return scene;
}
