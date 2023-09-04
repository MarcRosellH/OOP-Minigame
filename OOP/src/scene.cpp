#pragma once

#include "memleak.h"
#include "utils.h"

#include <glm/glm.hpp>
#include <iostream>
#include <stdlib.h>

#include "resource_manager.h"

#include "scene.h"
#include "object.h"
#include "camera.h"
#include "transform.h"

Scene::Scene(ResourceManager* _resource) : resource(_resource), object_count(0), total_count(0)
{
	root = DBG_NEW Object(this, true);

	camera = DBG_NEW Object(this, true);
	camera->add_component(COMPONENT_TYPE::COMPONENT_CAMERA);
	camera->start();
	Object* ref = root->add_child();
	ref->set_type(OBJECT_TYPE::ENEMY);
	ref->add_component(COMPONENT_TYPE::COMPONENT_TRANSFORM);
	ref->add_component(COMPONENT_TYPE::COMPONENT_MESH_RENDER)->start();
	ref->add_component(COMPONENT_TYPE::COMPONENT_BEHAVIOUR);
	ref->transform->set_position(glm::vec3(1.F, 1.F, 1.F));
	srand((unsigned)time(NULL));
}

Scene::~Scene()
{
}

bool Scene::update()
{
	for (int i = to_delete_objects.size()-1; i >= 0; i--)
	{
		to_delete_objects[i]->get_parent()->delete_child(to_delete_objects[i]);
		to_delete_objects.pop_back();
	}
	object_count = 0;
	root->update();
	camera->update();
	if (total_count != last_max_count)
	{
		system("cls");
		std::cout << total_count << std::endl;
	}
	last_max_count = total_count;
	resource->get_render_packs_vector()->clear();
	return true;
}

bool Scene::clean_up()
{
	//root->clean_up();
	RELEASE(root);

	//camera->clean_up();
	RELEASE(camera);

	return true;
}

bool Scene::delete_object(Object* _ref)
{
	Object* parent = _ref->get_parent();
	_ref->clean_up();
	parent->delete_child(_ref);
	RELEASE(_ref);
	return true;
}

ResourceManager* Scene::get_resource_manager() 
{
	return resource;
}