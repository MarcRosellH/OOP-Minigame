#pragma once
#ifndef __MESH_RENDER_H__
#define __MESH_RENDER_H__

#include <string>

#include "component.h"

class Model;

class ComponentMeshRender : public Component
{
public:
	// Public methods
	ComponentMeshRender(Object* _parent);
	virtual ~ComponentMeshRender();

	void start() override;
	void update(float dt) override;
	void clean_up() override;

	void request_model();

private:
	// Private methods

public:
	// Public data

private:
	// Private data
	unsigned int model_index;
	bool to_render;

};

#endif // !__MESH_RENDER_H__