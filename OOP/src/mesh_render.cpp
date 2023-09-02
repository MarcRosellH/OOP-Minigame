#pragma once

#include "memleak.h"

#include "scene.h"
#include "resource_manager.h"
#include "resources.h"

#include "mesh_render.h"
#include "object.h"
#include "component.h"
#include "transform.h"

ComponentMeshRender::ComponentMeshRender(Object* _parent) : Component(_parent), to_render(true), model_index(-1)
{
}

ComponentMeshRender::~ComponentMeshRender()
{
}

void ComponentMeshRender::start()
{
	request_model();
}

void ComponentMeshRender::update()
{
	if (to_render)
	{
		object->get_scene()->get_resource_manager()->add_render_pack(model_index, object->transform->matrix_position_rotation_scale());
	}
}

void ComponentMeshRender::clean_up()
{
}

void ComponentMeshRender::request_model()
{
	OBJECT_TYPE t = object->get_type();
	switch (t)
	{
	case OBJECT_TYPE::ENEMY:
		model_index = object->get_scene()->get_resource_manager()->get_model_index("Patrick.obj");
		break;
	case OBJECT_TYPE::BULLET:
		break;
	}
}
