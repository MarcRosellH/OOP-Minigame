#pragma once

#include "memleak.h"
#include "utils.h"

#include <glm/glm.hpp>

#include "resource_manager.h"

#include "scene.h"
#include "object.h"
#include "camera.h"
#include "transform.h"

Scene::Scene(ResourceManager* _resource) : resource(_resource)
{
	root = DBG_NEW Object(this, true);

	camera = DBG_NEW Object(this, true);
	camera->add_component(COMPONENT_TYPE::COMPONENT_CAMERA);
	camera->start();
	Object* ref = root->add_child();
	ref->set_type(OBJECT_TYPE::ENEMY);
	ref->add_component(COMPONENT_TYPE::COMPONENT_TRANSFORM);
	ref->add_component(COMPONENT_TYPE::COMPONENT_MESH_RENDER)->start();
	ref->transform->set_position(glm::vec3(1.F, 1.F, 1.F));
}

Scene::~Scene()
{
}

bool Scene::update()
{
	root->update();
	camera->update();
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