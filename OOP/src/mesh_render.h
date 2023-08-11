#pragma once
#ifndef __MESH_RENDER_H__
#define __MESH_RENDER_H__

#include "component.h"

class ComponentMeshRender : public Component
{
public:
	// Public methods
	ComponentMeshRender(Object* _parent);
	virtual ~ComponentMeshRender();

	void start() override;
	void update() override;
	void clean_up() override;

private:
	// Private methods

public:
	// Public data

private:
	// Private data

};

#endif // !__MESH_RENDER_H__